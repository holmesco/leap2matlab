///  leap2matlab.cpp
///  imports leap frames via mex struct
///  Connor Holmes
///  5/5/14

///currently returns hand position,velocity, and orientation data
///as well as the pointable data associated with each hand (pos, vel, dir)
///if no hands are present the function outputs a zero

///note: clear mex should be called before and after connection to leap

// Configured such that if the green Leap Light is towards user, the z-axis points towards the user
// the x-axis points to the right and the y axis points vertically upwards

// expects one output and no inputs

#include "leap2matlab.h"


//from Jeff Perry Matleap code
//makes a pointer to an allocated 3d vector based on the leap vector
mxArray *makeVector(const Leap::Vector& v)
{
	mxArray *p = mxCreateNumericMatrix (1, 3, mxDOUBLE_CLASS, mxREAL);
    *(mxGetPr (p) + 0) = v.x;
    *(mxGetPr (p) + 1) = v.y;
    *(mxGetPr (p) + 2) = v.z;
    return p;
}

//globally declare controller pointer so it can be deleted properly
// according to Jeff Perry, leap controller must be deallocated/destroyed only after
// all other destructors have been called; also controller required to be global
Leap::Controller *controller;

//called at exit from mex (mexAtExit)
//runs when clear mex is called
void closeLeap()
{
	//if locked, remove all locks
	while(mexIsLocked()) mexUnlock();
	mexPrintf("exiting mex...\n");
	if(controller){
		mexPrintf("deleting controller...\n");
		delete controller;
		controller = 0;
	}
	mexPrintf("Done\n");
	return;
	
}

//this function connects to a leap controller
void leapConnect(void)
{
    //Register Exit Function
    mexAtExit(closeLeap);

    //define leap controller and set up connection if no controller has yet been allocated
    //NOTE: controller is global so memory leak can occur if this check is not done
    if(controller) mexPrintf("Controller Already Connected\n");
    if(!controller){
        controller = new Leap::Controller;
        //wait for connection
        mexPrintf("Connecting to Leap Controller...\n");
        while(!controller->isConnected());
        mexPrintf("Connected!!!\n");
    }
    //return one for success
        
    return;
}

//function to get a frame and return it in the format acceptable to MATLAB
//ie mxArray
void loadFrame(mxArray *plhs[])
{
    //get current frame
	Leap::Frame frame = controller->frame();
	//DEBUG: check if frame is valid
	if(!frame.isValid()) mexErrMsgTxt("Frame returned was not valid");
    //wait for frame to be valid
	//while(!frame.isValid()) controller->frame();
    
    Leap::HandList hands = frame.hands();
    
    //import variables and create struct
	
	//number of hands
	int numHands = hands.count();
	
	//if there are no hands return a null pointer
	if(!numHands){
		plhs[0] = mxCreateDoubleScalar(0);
		return;
	}
	
	//fields returned to matlab in the struct:
	int numHandFields = 4;    //change number of fields here****
	const char *hand_field_names[] = 
	{
		"position",
		"velocity",
		"direction",
		"pointables"
		//add new hand fields here****
	};
	
	//pointable temp values for a given hand
	int numPointables;
	Leap::PointableList pointList;
	int numPointFields = 3;  	//change number of pointable fields here****
	const char *point_field_names[] = 
	{
		"position",
		"velocity",
		"direction"
		//add new pointable fields here****
	};
	mxArray *p;	
		
	//allocate the structure array to export to matlab
	mxArray *h = mxCreateStructMatrix(1, numHands, numHandFields, hand_field_names);
	
	//put vectors into structure array fields
	int i,j; //iterators
	for(i=0;i<numHands;i++){
		//fill in hand information
		mxSetFieldByNumber(h,i,0,makeVector(hands[i].palmPosition()));
		mxSetFieldByNumber(h,i,1,makeVector(hands[i].palmVelocity()));
		mxSetFieldByNumber(h,i,2,makeVector(hands[i].direction()));
		// add new field information here****
		//create pointable struct for this hand
		pointList = hands[i].pointables();		//get list of pointables for this hand
		numPointables = pointList.count();	    //get number of pointables for this hand
		//build list of pointables
		p = mxCreateStructMatrix(1,numPointables,numPointFields,point_field_names);
		for(j=0;j<numPointables;j++){
			mxSetFieldByNumber(p,j,0,makeVector(pointList[j].tipPosition()));
			mxSetFieldByNumber(p,j,1,makeVector(pointList[j].tipVelocity()));
			mxSetFieldByNumber(p,j,2,makeVector(pointList[j].direction()));
			//add new pointable fields here****
		}
		//put pointable list into hand struct
		mxSetFieldByNumber(h,i,3,p);
	}
    plhs[0] = h;  //set output
    return;
}
    
//gateway function to C++ from matlab
//essentially wrapper for commands
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	//check input parameters
	if(nrhs!=1) mexErrMsgTxt("Too Many or Too Few Input\n");
	
    char *command = mxArrayToString(prhs[0]);
    
	//if input is connect...connect to leap
    if (!strcmp(command, "connect") && !nlhs) leapConnect();
    else if (!strcmp(command, "getframe") && nlhs==1){
        if(controller->isConnected()) loadFrame(plhs);
        else mexPrintf("Controller must be connected before frame can be loaded");
    }else mexPrintf("Input command not valid or wrong number of output parameters\n");
    
	return;
	
}

	
	
