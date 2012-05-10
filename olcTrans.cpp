#include "stdafx.h"
#include <GL/glew.h>
#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#ifdef UNIX
#include <GL/glx.h>
#endif
#endif

// Includes
#include <iostream>
#include <string>
#include <stdio.h>

// Utilities, OpenCL and system includes
#include <oclUtils.h>
#include <shrQATest.h>

#if defined (__APPLE__) || defined(MACOSX)
#define GL_SHARING_EXTENSION "cl_APPLE_gl_sharing"
#else
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#endif

// Constants, defines, typedefs and global declarations
//*****************************************************************************
#define REFRESH_DELAY	  10 //ms

uint width = 512, height = 512;
size_t gridSize[2] = {width, height};

#define LOCAL_SIZE_X 16
#define LOCAL_SIZE_Y 16

float g_fBoxMin[] = {-1.0f, -1.0f, -1.0f,1.0f};
float g_fBoxMax[] = {1.0f, 1.0f, 1.0f,1.0f};



float density = 0.05f;
float brightness = 1.1f;
float transferOffset = 0.0f;
float transferScale = 1.0f;
bool linearFiltering = true;
bool ColorChange = false;
float* fColorTable;
int iTableSize = 0;

GLuint scalepbo = 0;                 // OpenGL pixel buffer object
GLuint transpbo = 0;                 // OpenGL pixel buffer object
GLuint transferpbo = 0;                 // OpenGL pixel buffer object
int iGLUTWindowHandle;          // handle to the GLUT window

// OpenCL vars
cl_platform_id cpPlatform;
cl_uint uiNumDevices;
cl_device_id* cdDevices;
cl_uint uiDeviceUsed;
cl_uint uiDevCount;
cl_context cxGPUContext;
cl_device_id device;
cl_command_queue cqCommandQueue;
cl_program cpProgram;
cl_kernel ckKernel;
cl_kernel ScalseKernel;
cl_kernel TransformKernel;
cl_kernel LongToShortKernel;
cl_int ciErrNum;
cl_mem pbo_cl;
cl_mem d_volumeArray;
cl_mem d_transferFuncArray;
cl_mem d_invViewMatrix;
cl_mem d_boxmin;
cl_mem d_boxmax;
char* cPathAndName = NULL;          // var for full paths to data, src, etc.
char* cSourceCL;                    // Buffer to hold source for compilation 
const char* cExecutableName = NULL;
cl_bool g_bImageSupport;
cl_sampler volumeSamplerLinear;
cl_sampler volumeSamplerNearest;
cl_sampler transferFuncSampler;
cl_bool g_glInterop = false;

int iFrameCount = 0;                // FPS count for averaging
int iFrameTrigger = 20;             // FPS trigger for sampling
int iFramesPerSec = 0;              // frames per second
int g_Index = 0;
bool g_bFBODisplay = false;
int ox, oy;                         // mouse location vars
int buttonState = 0;       
cl_mem d_scalepoint;
cl_mem d_transpoint;
cl_mem d_transferpoint;
BYTE *p_outpoint;
BYTE *p_outByte;

// Forward Function declarations
//*****************************************************************************
// OpenCL Functions
void initScalePixelBuffer(int Width,int Height);
void initTransPixelBuffer(int Width,int Height);
void initTransferPixelBuffer(int Width,int Height);
void createCLContext();

// OpenGL functionality
void timerEvent(int value);
// Helpers
void Cleanup(int iExitCode);
void (*pCleanup)(int) = &Cleanup;


int InitOpenCLContext() 
{
	// start logs
	shrSetLogFileName ("oclVolumeRender.txt");

	// get command line arg for quick test, if provided
	// process command line arguments

	// First initialize OpenGL context, so we can properly setup the OpenGL / OpenCL interop.

// 	glewInit();
// 	GLboolean bGLEW = glewIsSupported("GL_VERSION_2_0 GL_ARB_pixel_buffer_object"); 
// 	oclCheckErrorEX(bGLEW, shrTRUE, pCleanup);
	g_glInterop = true;


	// Create OpenCL context, get device info, select device, select options for image/texture and CL-GL interop
	createCLContext();

	// Print device info
	clGetDeviceInfo(cdDevices[uiDeviceUsed], CL_DEVICE_IMAGE_SUPPORT, sizeof(g_bImageSupport), &g_bImageSupport, NULL);
	//shrLog("%s...\n\n", g_bImageSupport ? "Using Image (Texture)" : "No Image (Texuture) Support");      
//	shrLog("Detailed Device info:\n\n");
	oclPrintDevInfo(LOGBOTH, cdDevices[uiDeviceUsed]);

	// create a command-queue
	cqCommandQueue = clCreateCommandQueue(cxGPUContext, cdDevices[uiDeviceUsed], 0, &ciErrNum);
	oclCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);

	// Program Setup
	size_t program_length;
	cPathAndName = shrFindFilePath("Transform.cl", ".");
	oclCheckErrorEX(cPathAndName != NULL, shrTRUE, pCleanup);
	cSourceCL = oclLoadProgSource(cPathAndName, "", &program_length);
	oclCheckErrorEX(cSourceCL != NULL, shrTRUE, pCleanup);

	// create the program
	cpProgram = clCreateProgramWithSource(cxGPUContext, 1,
		(const char **)&cSourceCL, &program_length, &ciErrNum);
	oclCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);

	// build the program
	std::string buildOpts = "-cl-single-precision_constant";
//	buildOpts += g_bImageSupport ? " -DIMAGE_SUPPORT" : "";
//	ciErrNum = clBuildProgram(cpProgram, 1, &cdDevices[uiDeviceUsed],"-cl-fast-relaxed-math", NULL, NULL);
	ciErrNum = clBuildProgram(cpProgram, 1, &cdDevices[uiDeviceUsed],NULL, NULL, NULL);
	if (ciErrNum != CL_SUCCESS)
	{
		// write out standard error, Build Log and PTX, then cleanup and return error
		shrLogEx(LOGBOTH | ERRORMSG, ciErrNum, STDERROR);
		oclLogBuildInfo(cpProgram, oclGetFirstDev(cxGPUContext));
		oclLogPtx(cpProgram, oclGetFirstDev(cxGPUContext), "oclVolumeRender.ptx");
		Cleanup(EXIT_FAILURE); 
	}

	// create the kernel
	ScalseKernel = clCreateKernel(cpProgram, "d_render", &ciErrNum);
	oclCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);

	TransformKernel = clCreateKernel(cpProgram, "angle", &ciErrNum);
	oclCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);

	LongToShortKernel = clCreateKernel(cpProgram, "transfer", &ciErrNum);
	oclCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);
	return TRUE;
}

// Intitialize OpenCL
//*****************************************************************************
void createCLContext() {
    //Get the NVIDIA platform
    ciErrNum = oclGetPlatformID(&cpPlatform);
    oclCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);

    // Get the number of GPU devices available to the platform
    ciErrNum = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 0, NULL, &uiDevCount);
    oclCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);

    // Create the device list
    cdDevices = new cl_device_id [uiDevCount];
    ciErrNum = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, uiDevCount, cdDevices, NULL);
    oclCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);

    // Get device requested on command line, if any
    uiDeviceUsed = 0;
    unsigned int uiEndDev = uiDevCount - 1;
 

	// Check if the requested device (or any of the devices if none requested) supports context sharing with OpenGL
    if(0)
    {
        bool bSharingSupported = false;
        for(unsigned int i = uiDeviceUsed; (!bSharingSupported && (i <= uiEndDev)); ++i) 
        {
            size_t extensionSize;
            ciErrNum = clGetDeviceInfo(cdDevices[i], CL_DEVICE_EXTENSIONS, 0, NULL, &extensionSize );
            oclCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);
            if(extensionSize > 0) 
            {
                char* extensions = (char*)malloc(extensionSize);
                ciErrNum = clGetDeviceInfo(cdDevices[i], CL_DEVICE_EXTENSIONS, extensionSize, extensions, &extensionSize);
                oclCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);
                std::string stdDevString(extensions);
                free(extensions);

                size_t szOldPos = 0;
                size_t szSpacePos = stdDevString.find(' ', szOldPos); // extensions string is space delimited
                while (szSpacePos != stdDevString.npos)
                {
                    if( strcmp(GL_SHARING_EXTENSION, stdDevString.substr(szOldPos, szSpacePos - szOldPos).c_str()) == 0 ) 
                    {
                        // Device supports context sharing with OpenGL
                        uiDeviceUsed = i;
                        bSharingSupported = true;
                        break;
                    }
                    do 
                    {
                        szOldPos = szSpacePos + 1;
                        szSpacePos = stdDevString.find(' ', szOldPos);
                    } 
                    while (szSpacePos == szOldPos);
                }
            }
        }
       
        // Log CL-GL interop support and quit if not available (sample needs it)
 //       shrLog("%s...\n", bSharingSupported ? "Using CL-GL Interop" : "No device found that supports CL/GL context sharing");  
        oclCheckErrorEX(bSharingSupported, true, pCleanup);

        // Define OS-specific context properties and create the OpenCL context
        #if defined (__APPLE__)
            CGLContextObj kCGLContext = CGLGetCurrentContext();
            CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
            cl_context_properties props[] = 
            {
                CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup, 
                0 
            };
            cxGPUContext = clCreateContext(props, 0,0, NULL, NULL, &ciErrNum);
        #else
            #ifdef UNIX
                cl_context_properties props[] = 
                {
                    CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(), 
                    CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(), 
                    CL_CONTEXT_PLATFORM, (cl_context_properties)cpPlatform, 
                    0
                };
                cxGPUContext = clCreateContext(props, 1, &cdDevices[uiDeviceUsed], NULL, NULL, &ciErrNum);
            #else // Win32
                cl_context_properties props[] = 
                {
                    CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(), 
                    CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(), 
                    CL_CONTEXT_PLATFORM, (cl_context_properties)cpPlatform, 
                    0
                };
                cxGPUContext = clCreateContext(props, 1, &cdDevices[uiDeviceUsed], NULL, NULL, &ciErrNum);
            #endif
        #endif
    }
    else 
    {
		// No GL interop
        cl_context_properties props[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)cpPlatform, 0};
        cxGPUContext = clCreateContext(props, 1, &cdDevices[uiDeviceUsed], NULL, NULL, &ciErrNum);

		g_glInterop = false;
    }
    oclCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);
}

void initScalePixelBuffer(int Width,int Height)
{
	ciErrNum = CL_SUCCESS;

	if (scalepbo) {
		// delete old buffer
		clReleaseMemObject(d_scalepoint);
		delete[] p_outpoint;
		glDeleteBuffersARB(1, &scalepbo);
	}

	d_scalepoint = clCreateBuffer(cxGPUContext, CL_MEM_ALLOC_HOST_PTR, Width * Height * sizeof(BYTE), NULL, &ciErrNum);
	p_outpoint = new BYTE[Height*Width];

}

void initTransPixelBuffer(int Width,int Height)
{
	ciErrNum = CL_SUCCESS;

	if (transpbo) {
		// delete old buffer
		clReleaseMemObject(d_transpoint);
		delete[] p_outpoint;
		glDeleteBuffersARB(1, &transpbo);
	}
	d_transpoint = clCreateBuffer(cxGPUContext, CL_MEM_READ_WRITE, Width * Height * sizeof(unsigned char), NULL, &ciErrNum);
	p_outpoint = new BYTE[Height*Width];
}

void initTransferPixelBuffer(int Width,int Height)
{
	ciErrNum = CL_SUCCESS;

	if (transferpbo) {
		// delete old buffer
		clReleaseMemObject(d_transferpoint);
		delete[] p_outpoint;
		glDeleteBuffersARB(1, &transferpbo);
	}
	unsigned char* temp = new unsigned char[Width * Height];
	d_transferpoint = clCreateBuffer(cxGPUContext, CL_MEM_READ_WRITE, Width * Height * sizeof(unsigned char), NULL, &ciErrNum);
//	p_outpoint = new BYTE[Height*Width];
}

void GetScale()
{

}

void GetTrans(unsigned char* pData,BYTE* pDest,int Width,int Height,float angle)
{
	ciErrNum = CL_SUCCESS;     
	cl_mem pInput;
	unsigned char* temp = new unsigned char[Width * Height];
	pInput = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY, Width * Height * sizeof(unsigned char), pData, &ciErrNum);
	clEnqueueWriteBuffer(cqCommandQueue,pInput,CL_FALSE,0,Width * Height * sizeof(unsigned char),pData,0,NULL,NULL);
	ciErrNum |= clSetKernelArg(TransformKernel, 1, sizeof(cl_mem), (void *) &pInput);
	//test
	//	cl_mem inttest = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY, Width * Height * sizeof(unsigned int), pData, &ciErrNum);
	ciErrNum |= clSetKernelArg(TransformKernel, 0, sizeof(cl_mem), (void *) &d_transpoint);

	ciErrNum |= clSetKernelArg(TransformKernel, 2, sizeof(int), &Width);
	ciErrNum |= clSetKernelArg(TransformKernel, 3, sizeof(int), &Height);
	ciErrNum |= clSetKernelArg(TransformKernel, 4, sizeof(float), &angle);

	size_t localSize[] = {LOCAL_SIZE_X,LOCAL_SIZE_Y};
	size_t gridSize[2] = {Width, Height};
	ciErrNum = clEnqueueNDRangeKernel(cqCommandQueue, TransformKernel, 2, NULL, gridSize,NULL,  0, 0, 0);
	ciErrNum = clFinish(cqCommandQueue);

	ciErrNum = clEnqueueReadBuffer(cqCommandQueue, d_transpoint, CL_FALSE, 0, sizeof(unsigned char) * height * width, pDest, 0, NULL, NULL);
}

void GetTransfer(unsigned short* pData,BYTE* pDest,int Width,int Height,int wc,int ww)
{
	ciErrNum = CL_SUCCESS;     
	cl_mem pInput;
	unsigned char* temp = new unsigned char[Width * Height];
	pInput = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY, Width * Height * sizeof(unsigned short), pData, &ciErrNum);
	clEnqueueWriteBuffer(cqCommandQueue,pInput,CL_FALSE,0,Width * Height * sizeof(unsigned short),pData,0,NULL,NULL);
//	pInput = clCreateBuffer(cxGPUContext, CL_MEM_READ_WRITE, Width * Height * sizeof(unsigned short), pData, &ciErrNum);
	ciErrNum |= clSetKernelArg(LongToShortKernel, 1, sizeof(cl_mem), (void *) &pInput);
	//test
//	cl_mem inttest = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY, Width * Height * sizeof(unsigned int), pData, &ciErrNum);
	ciErrNum |= clSetKernelArg(LongToShortKernel, 0, sizeof(cl_mem), (void *) &d_transferpoint);
	ciErrNum |= clSetKernelArg(LongToShortKernel, 2, sizeof(int), &wc);
	ciErrNum |= clSetKernelArg(LongToShortKernel, 3, sizeof(int), &ww);

	ciErrNum |= clSetKernelArg(LongToShortKernel, 4, sizeof(int), &Width);
	ciErrNum |= clSetKernelArg(LongToShortKernel, 5, sizeof(int), &Height);

	//test
	//	cl_mem inttest = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY, Width * Height * sizeof(unsigned int), pData, &ciErrNum);

	size_t localSize[] = {LOCAL_SIZE_X,LOCAL_SIZE_Y};
	size_t gridSize[2] = {Width, Height};
	ciErrNum = clEnqueueNDRangeKernel(cqCommandQueue, LongToShortKernel, 2, NULL, gridSize,NULL,  0, 0, 0);
	ciErrNum = clFinish(cqCommandQueue);

	ciErrNum = clEnqueueReadBuffer(cqCommandQueue, d_transferpoint, CL_FALSE, 0, sizeof(unsigned char) * height * width, pDest, 0, NULL, NULL);

	//for test
// 	int *testint = new int[Width * Height];
// 	for(int i = 0;i<Width * Height;i++)
// 	{
// 		testint[i] = 200;
// 	}
// 	cl_mem test;
// 	test = clCreateBuffer(cxGPUContext, CL_MEM_READ_WRITE, Width * Height * sizeof(int), testint, &ciErrNum);
// 	clEnqueueWriteBuffer(cqCommandQueue,test,CL_FALSE,0,Width * Height * sizeof(int),testint,0,NULL,NULL);
// 	int *testout = new int[Width * Height];
// 	memset(testout,0,sizeof(int)*Width * Height);
// 	ciErrNum = clEnqueueReadBuffer(cqCommandQueue, test, CL_TRUE, 0, sizeof(int) * height * width, testout, 0, NULL, NULL);

}

void Cleanup(int iExitCode)
{
	// cleanup allocated objects
	//shrLog("\nStarting Cleanup...\n\n");
	if(cPathAndName)free(cPathAndName);
	if(cSourceCL)free(cSourceCL);
	if(ScalseKernel)clReleaseKernel(ScalseKernel);  
	if(TransformKernel)clReleaseKernel(TransformKernel); 
	if(LongToShortKernel)clReleaseKernel(LongToShortKernel); 
	if(cpProgram)clReleaseProgram(cpProgram);
	if(volumeSamplerLinear)clReleaseSampler(volumeSamplerLinear);
	if(volumeSamplerNearest)clReleaseSampler(volumeSamplerNearest);
	if(transferFuncSampler)clReleaseSampler(transferFuncSampler);
	if(d_volumeArray)clReleaseMemObject(d_volumeArray);
	if(d_transferFuncArray)clReleaseMemObject(d_transferFuncArray);
	if(pbo_cl)clReleaseMemObject(pbo_cl);    
	if(d_transferpoint)clReleaseMemObject(d_transferpoint);
	if(d_transpoint)clReleaseMemObject(d_transpoint);
	if(d_scalepoint)clReleaseMemObject(d_scalepoint);

	if(d_invViewMatrix)clReleaseMemObject(d_invViewMatrix);    
	if(cqCommandQueue)clReleaseCommandQueue(cqCommandQueue);
	if(cxGPUContext)clReleaseContext(cxGPUContext);
	if(d_boxmax)clReleaseMemObject(d_boxmax);
	if (d_boxmin)clReleaseMemObject(d_boxmin);


	// finalize logs and leave
	shrLogEx(LOGBOTH | CLOSELOG, 0, "%s Exiting...\nPress <Enter> to Quit\n", cExecutableName);
#ifdef WIN32
	getchar();
#endif
	exit (iExitCode);
}