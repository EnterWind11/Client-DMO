// Copyright (C) 1998-2007 Alias | Wavefront, a division of Silicon Graphics
// Limited.
//
// The information in this file is provided for the exclusive use of the
// licensees of Alias | Wavefront.  Such users have the right to use, modify,
// and incorporate this code into other products for purposes authorized
// by the Alias | Wavefront license agreement, without fee.
//
// ALIAS | WAVEFRONT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
// INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
// EVENT SHALL ALIAS | WAVEFRONT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
// CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
// DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

//
// $Revision: /main/18 $
// $Date: 2000/04/19 15:31:12 $
//


// Precompiled Headers
#include "MDtAPIPCH.h"


//
//  Required functions form other modules.
//  Need to call in the order used in DtExt_Init
//  in order to setup the working environment properly.
//
extern void lightNew();
extern void cameraNew();
extern void shapeNew(void);
extern void mtlNew();
extern void DtMtlsUpdate();
extern void gMDtObjectSetParents(void);


// Private data.
//
static char *out_dir  = NULL; // The output directory for exporting.
static char *out_name = NULL; // The output name for exporting.

// The Scene module data structure.
//
typedef struct
{
    char    *name;
    int     frame_start;
    int     frame_end;
    int     frame_by;
    int     frame_current;
    double  start;
    double  end;
    double  by;
    int     keyCount;    // Number of key frames.
    int     *keyFrames;  // Array of key frames.
    int     type;        // DtStatic, DtAnimation, DtKinematics
} DtPrivate;

// By default: export static scene
//
const int kFrameStartDefault = 0;
const int kFrameEndDefault = 0;
const int kFrameByDefault = 0;

static DtPrivate *local = NULL;


//  Forward declarations.
//
void  sceneNew(void);
void  sceneSetName( char * );
void  sceneCreateCaches(void);

// Private data initialization:
//
static  int _outputTransforms = kTRANSFORMALL;
static  int _outputPolys = kTESSTRI;
static  int _materialInventory = FALSE;
static  int _xTextureRes = 256;
static  int _yTextureRes = 256;
static  int _MaxxTextureRes = 4096;
static  int _MaxyTextureRes = 4096;
static  int _inlineTextures = TRUE;
static  int _outputCameras = FALSE;
static  int _outputParents = TRUE;
static  int _softTextures = FALSE;
static  int _walkMode = ALL_Nodes;
static  int _debugoutput = FALSE;
static  int _updateShaders = 0;
static  int _reverseWinding = 0;
static  char *textureSearchPath = NULL;
static  int _rescaleRange = 0;
static  int _vertexAnimation = TRUE;
static  int _jointHierarchy = TRUE;
static  int _multiTexture = FALSE;
static  int _ExportNonVisible = FALSE;
static  int _originalTexture = FALSE;
static  char *_dtAPIVersion="203";
static  bool _HaveGui = true;

char *DtAPIVersion()
{
    return _dtAPIVersion;
}
    
int
DtExt_JointHierarchy()
{
    return _jointHierarchy;
}

void
DtExt_setJointHierarchy( int mode )
{
    _jointHierarchy = mode;
}

void
DtExt_setExportNonVisible( int mode )
{
    _ExportNonVisible = mode;
}

int
DtExt_getExportNonVisible()
{
    return _ExportNonVisible;
}

int
DtExt_MultiTexture()
{
    return _multiTexture;
}

void
DtExt_setMultiTexture( int mode )
{
    _multiTexture = mode;
}

int
DtExt_OriginalTexture()
{
    return _originalTexture;
}

void
DtExt_setOriginalTexture( int mode )
{
    _originalTexture = mode;
}

int
DtExt_VertexAnimation()
{
    return _vertexAnimation;
}   

void
DtExt_setVertexAnimation( int mode )
{
    _vertexAnimation = mode;
}   

int 
DtExt_WalkMode()
{
    return _walkMode;
}

void 
DtExt_setWalkMode( int mode )
{
    _walkMode = mode;
}

int 
DtExt_softTextures()
{
    return _softTextures;
}

void 
DtExt_setSoftTextures( int mode )
{
    _softTextures = mode;
}

int 
DtExt_tesselate()
{
    return _outputPolys;
}

void 
DtExt_setTesselate( int mode )
{
    _outputPolys = mode;
}


int 
DtExt_materialInventory()
{
    return _materialInventory;
}

void    
DtExt_setOutputTransforms( int mode )
{
    _outputTransforms = mode;
}

int     
DtExt_outputTransforms()
{
    return  _outputTransforms;
}


int     
DtExt_xTextureRes()
{
    return  _xTextureRes;
}

int     
DtExt_yTextureRes()
{
    return  _yTextureRes;
}

void    
DtExt_setXTextureRes( int res )
{
    _xTextureRes = res;
}

void    
DtExt_setYTextureRes( int res )
{
    _yTextureRes = res;
}

int
DtExt_MaxXTextureRes()
{
    return  _MaxxTextureRes;
}

int
DtExt_MaxYTextureRes()
{
    return  _MaxyTextureRes;
}

void
DtExt_setMaxXTextureRes( int res )
{
  _MaxxTextureRes = res;
}

void
DtExt_setMaxYTextureRes( int res )
{
  _MaxyTextureRes = res;
}

void    
DtExt_setInlineTextures( int status )
{
    _inlineTextures = status;
}

int   
DtExt_inlineTextures()
{
    return _inlineTextures;
}

void    
DtExt_setOutputCameras( int status )
{
    _outputCameras = status;
}

int   
DtExt_outputCameras()
{
    return _outputCameras;
}

void    
DtExt_setParents( int status )
{
    _outputParents = status;
}

int     
DtExt_Parents()
{
    return _outputParents;
}

void 
DtExt_setDebug( int status )
{
    _debugoutput = status;
}

int 
DtExt_Debug()
{
    return _debugoutput;
}

void 
DtSetUpdateShaders(int LStatus)
{
 _updateShaders=LStatus;
}

int 
DtGetUpdateShaders()
{
 return(_updateShaders);
}

void
DtExt_setWinding( int winding )
{
    _reverseWinding = winding;
}

int
DtExt_Winding()
{
    return _reverseWinding;
}

void
DtExt_setRescaleRange( int flag )
{
    _rescaleRange = flag;
}

int
DtExt_RescaleRange()
{
    return _rescaleRange;
}

void
DtExt_addTextureSearchPath( char *tPath )
{
    if ( tPath && *tPath )
    {
        if ( textureSearchPath )
        {
            unsigned int uiLen = 
                strlen( textureSearchPath ) + strlen( tPath ) + 1;
            textureSearchPath = (char *)NiRealloc( textureSearchPath, uiLen );
            NiStrcat( textureSearchPath, uiLen, tPath );
        } 
        else 
        {
            textureSearchPath = NiStrdup( tPath );
        }
    }

}

char *
DtExt_getTextureSearchPath()
{
    return textureSearchPath;
}

bool DtExt_HaveGui()
{
    return _HaveGui;
}

void DtExt_setHaveGui(bool isOn)
{
    _HaveGui = isOn;
}


//
// Msg( format, arg1, arg2, ... )
//
void
DtExt_Msg( char *fmt, ... )
{
    va_list  args;
    char     buffer[2048];

    if( DtExt_Debug() )
    {
        // Print out the message.
        // 
        va_start( args, fmt );
        (void) NiVsprintf( buffer, 2048, fmt, args );
        va_end( args );

        fprintf(stderr, "%s", buffer );
        cerr << buffer << flush;

    }
}

//
// Err( format, arg1, arg2, ... )
//
void
DtExt_Err( char *fmt, ... )
{
    va_list  args;
    char     buffer[2048];

    // Print out the message.
    //
    va_start( args, fmt );
    (void) NiVsprintf( buffer, 2048, fmt, args );
    va_end( args );

//    fprintf(stderr, "%s", buffer );

    cerr << buffer << flush;
}


//
//  DtExt_nodeName - return string containing the "type of node".
//
#if 0
const char* DtExt_nodeName( AlObject *node )
{
    if( NULL == node )
    {
        return "null node";
    }
    switch ( node->type( ))
    {
        case kAmbientLightType:       return "AlAmbientLight";
        case kAreaLightType:          return "AlAreaLight";
        case kCameraEyeType:
        case kCameraViewType:
        case kCameraUpType:           return "AlCameraNode";
        case kCameraType:             return "AlCamera";
        case kClusterType:            return "AlCluster";
        case kClusterNodeType:        return "AlClusterNode";
        case kClusterMemberType:      return "AlClusterMember";
        case kCurveNodeType:          return "AlCurveNode";
        case kCurveType:              return "AlCurve";
        case kCurveCVType:            return "AlCurveCV";
        case kCurveOnSurfaceType:     return "AlCurveOnSurface";
        case kDagNodeType:            return "AlDagNode";
        case kDirectionLightType:     return "AlDirectionLight";
        case kFaceNodeType:           return "AlFaceNode";
        case kFaceType:               return "AlFace";
        case kGroupNodeType:          return "AlGroupNode";
        case kLightLookAtNodeType:    return "AlLightLookAtNode";
        case kLightNodeType:          return "AlLightNode";
        case kLightType:              return "AlLight";
        case kLightUpNodeType:        return "AlLightUpNode";
        case kLinearLightType:        return "AlLinearLight";
        case kNonAmbientLightType:    return "AlNonAmbientLight";
        case kPointLightType:         return "AlPointLight";
        case kSpotLightType:          return "AlSpotLight";
        case kShellNodeType:          return "AlShellNode";
        case kShellType:              return "AlShell";
        case kSurfaceNodeType:        return "AlSurfaceNode";
        case kSurfaceType:            return "AlSurface";
        case kSurfaceCVType:          return "AlSurfaceCV";
        case kSetType:                return "AlSet";
        case kSetMemberType:          return "AlSetMember";
        case kChannelType:            return "AlChannel";
        case kActionType:             return "AlAction";
        case kMotionActionType:       return "AlMotionAction";
        case kParamActionType:        return "AlParamAction";
        case kKeyframeType:           return "AlKeyframe";
        case kStreamType:             return "AlStream";
        case kShaderType:             return "AlShader";
        case kTextureType:            return "AlTexture";
        case kEnvironmentType:        return "AlEnvironment";
        case kPolysetNodeType:        return "AlPolysetNode";
        case kPolysetType:            return "AlPolyset";
        case kPolygonType:            return "AlPolygon";
        case kPolysetVertexType:      return "AlPolysetVertex";
        case kAttributeType:          return "AlAttribute";
        case kArcAttributeType:       return "AlArcAttribute";
        case kLineAttributeType:      return "AlLineAttribute";
        case kCurveAttributeType:     return "AlCurveAttribute";
        case kPlaneAttributeType:     return "AlPlaneAttribute";
        case kConicAttributeType:     return "AlConicAttribute";
        case kRevSurfAttributeType:   return "AlRevSurfAttribute";
        case kTextureNodeType:        return "AlTextureNode";
        default:                      return "unknownClassName";
    }
}
#endif

//  ========== DtExt_SceneInit ==========
//
//  SYNOPSIS
//  Setup the scene structures for further calls to the Dt layer.
//
void  DtExt_SceneInit( char *name )
{
    MGlobal::MMayaState state = MGlobal::mayaState();

    if(state == MGlobal::kBatch)
    {
        DtExt_setHaveGui(false);
    }
    else
    {
        DtExt_setHaveGui(true);
    }

    // Now need to create the Dt database.
    //
    sceneNew();

    // Now need to setup the output file/scene name.
    //
    sceneSetName( name );
}


//  ========== DtExt_dbInit ==========
//
//  SYNOPSIS
//  Setup the database for further calls to the Dt layer
//
//  In DtExt_.h:
//
void DtExt_dbInit( ) 
{
    // Call all of the routines needed to setup the database.
    //
    DtExt_setOutputCameras( 1 ); // for debugging turn this on

    int body = 0;
    int frameCnt = local->frame_end - local->frame_start;
    if( (0 != body) && (0 != frameCnt) )
    {
        local->type = DtKinematic;
    }
    else if( 0 != frameCnt )
    {
        local->type = DtAnimation;
    }
    else
    {
        local->type = DtStatic;
    }

    // The order of the following function calls is important.
    //
    gPhysXClothConstrNew();
    gPhysXJointNew();
    gPhysXShapeNew();
    gParticleSystemNew();
    gLevelOfDetailNew();
    lightNew();          // in doLight.c++
    cameraNew();         // in doCamera.c++
    shapeNew();          // in doShape.c++
    gMDtObjectSetParents();
    mtlNew();            // in doMaterial.c++
    gAnimDataAnimNew();
    gIKDataNew();

    gParticleSystemManager.SetParticleMaterials();
}

//  ========== DtExt_CleanUp ==========
//
//  SYNOPSIS
//  Setup the database for further calls to the Dt layer
//

extern void DtExt_LightDelete( void );
extern void DtExt_CameraDelete( void );
extern void DtExt_ShapeDelete( void );
extern void DtExt_MaterialDelete( void );

void  DtExt_CleanUp()
{
    DtExt_LightDelete();
    DtExt_CameraDelete();
    DtExt_ShapeDelete();
    DtExt_MaterialDelete();

    gAnimDataReset();
    gJointXformMan.Reset();
    gMultParentMan.Reset();

    gMDtObjectReset();

    if ( textureSearchPath )
    {
        NiFree( textureSearchPath );
        textureSearchPath = NULL;
    }

    if( local )
    {
        if (local->name)
        {
            NiFree( local->name);
            local->name = NULL;
        }

        NiFree( local );
        local = NULL;
    }

    if (out_dir)
    {
        NiFree(out_dir);
        out_dir = NULL;
    }

    gParticleSystemManager.Reset();
}

/* ================================================================ *
 * -------------------  PUBLIC  FUNCTIONS  ------------------------ *
 * ================================================================ */


//  ========== DtOpenModel ==========
//
//  SYNOPSIS
//  Returns true if running under Open Model. 
//
int  DtOpenModel( void )
{
    // We are not running under OpenModel.
    //
    return( 0 );
}  // DtOpenModel //



//  ========== DtPrintf ==========
//
//  SYNOPSIS
//  A 'printf' style output function.
//
//
void DtPrintf(DtOutputType ot, const char* fmt, ... )
{
     va_list ap;

     va_start(ap,fmt);
     switch(ot) {
         case dStdout: fprintf(stdout,fmt,ap);
                       break;
         case dStderr: fprintf(stderr,fmt,ap);
                       break;
         case dErrlog: fprintf(stderr,fmt,ap);
                       break;
     }
     va_end(ap);
}


//  ========== DtSceneGetName ==========
//
//  SYNOPSIS
//  Returns a pointer to the scene name. This is an
//  internal buffer and should not be changed.
//
void  DtSceneGetName( char **name )
{
    if( NULL == local ) 
    {   
        return;
    }
    *name = local->name;
    return;

}  // DtSceneGetName //


//  ========== DtSceneGetType ==========
//
//  SYNOPSIS
//  Returns the type of scene currently loaded.
//      The scene types are:
//
//      DtStatic    : static scene, no animation.
//      DtAnimation : animation with no kinematics
//      DtKinematic : Animation of kinematic scene includes joint angles.
//
int  DtSceneGetType( void )
{
    if( NULL == local ) 
    {
        return(0);
    }
    return local->type;

}  // DtSceneGetType //


//
//  ========== DtFrameSet ==========
//
//  SYNOPSIS
//  This function is called by the DSO during the export translation. 
//  It initiates the needed actions to update the scene graph to the 
//  given frame. All animated channel data will be current
//  to the give frame after this call.
//

int  DtFrameSet( int frame )
{
    double frame_use;
    
    // Check for valid frame.
    //
    if( ( local->frame_start > frame ) || 
        ( local->frame_end < frame ) ) 
    {
        return 0;
    }

    // We should update the scene graph with the new channel data.
    // Sets the global time to the specified time.
    //
    frame_use = frame;
    MGlobal::viewFrame( frame_use );

    local->frame_current = frame;

    //  If we are not outputting transforms then assume the vertex data
    //  is in world space and we need to refresh the data.
    //

    // We will always try to do this.

    DtExt_SetupWorldVertices();

    // Here we will go thru the materials again and update the 
    // data

    if ( _updateShaders )
        DtMtlsUpdate();

    return 1;

}  // DtFrameSet

/*
 *  ========== DtFrameGet ==========
 *
 *  SYNOPSIS
 *  This function is called by the DSO to get the
 *      current frame number.
 */
int  DtFrameGet( void)
{
    return local->frame_current;

}  /* DtFrameGet */


/*
 *  ========== DtFrameGetCount ==========
 *
 *  SYNOPSIS
 *  Returns the total number of frames in the animation.
 */

int  DtFrameGetCount()
{
    return( local->frame_end - local->frame_start );

}  /* DtFrameGetCount */


/*
 *  ========== DtFrameGetStart ==========
 *
 *  SYNOPSIS
 *  Returns the frame number of the first frame in the animation.
 */

int  DtFrameGetStart()
{
    return( local->frame_start );

}  /* DtFrameGetStart */

/*
 *  ========== DtFrameSetStart ==========
 *
 *  SYNOPSIS
 *  Set the frame number of the first frame in the animation to use.
 */

void  DtFrameSetStart( int start )
{
    local->frame_start = start;

}  /* DtFrameSetStart */


/*
 *  ========== DtFrameGetEnd ==========
 *
 *  SYNOPSIS
 *  Returns the frame number of the last frame in the animation.
 */

int  DtFrameGetEnd(void)
{
    return( local->frame_end );

}  /* DtFrameGetEnd */

/*
 *  ========== DtFrameSetEnd ==========
 *
 *  SYNOPSIS
 *  Sets the frame number of the last frame in the animation to use.
 */

void  DtFrameSetEnd( int end )
{
    local->frame_end = end;

}  /* DtFrameSetEnd */

/*
 *  ========== DtFrameSetBy ==========
 *
 *  SYNOPSIS
 *  Sets the frame 'by' value which determines intervals of frame samples.
 */

void DtFrameSetBy( int by )
{
    local->frame_by = by;
} /* DtFrameSetBy */

/*
 *  ========== DtFrameGetBy ==========
 *
 *  SYNOPSIS
 *  Returns the frame 'by' value which determines intervals of frame samples.
 */
int DtFrameGetBy( void )
{
    return(local->frame_by);
} /* DtFrameGetBy */

/*
 *  ========== DtFrameGetRange ==========
 *
 *  SYNOPSIS
 *  Returns the frame number of the first and last frame in the animation.
 */

int  DtFrameGetRange( int *start, int *end, int *by )
{
    *start = (int)local->start;
    *end   = (int)local->end;
    *by    = (int)local->by;

    return( 1 );

}  /* DtFrameGetRange */

//  ========== DtFrameGetKeyFrames ==========
//
//  SYNOPSIS
//  Return key frames set by the user.
//
//  The number of key frames in returned in "count". A read-only
//  pointer to the array of key frames is returned in "frames".
//  Key frames are integers within the current frame range.
//
void  DtFrameGetKeyFrames( int *count, int **frames )
{
    *count  = local->keyCount;
    *frames = local->keyFrames;

}  // DtFrameGetKeyFrames


/*
 *  ========== DtGetFilename ==========
 *
 *  SYNOPSIS
 *  Returns the selected output directory and basename
 *  of the output files to be created by the export DSO.
 */

int  DtGetFilename(char **basename)
{
    if (out_name == NULL) return(0);

    *basename = out_name;
    return(1);

}  /* DtGetFilename */


/*
 *  ========== DtGetDirectory ==========
 *
 *  SYNOPSIS
 *  Returns the output directory name.
 */

int  DtGetDirectory(char **directory)
{

    if (out_dir == NULL) return(0);

    *directory = out_dir;
    return(1);

}  /* DtGetDirectory */


//  ========== DtSetParent ==========
//
//  SYNOPSIS
//  Set the top level widget. Useful for DSO writers
//  who wish to post a dialog. Use the returned widget
//  as the parent of the dialog.

#ifdef WIN32
static int dtParentWidget = NULL;
#else
static Widget dtParentWidget = NULL;
#endif

#ifdef WIN32
void DtExt_SetParentWidget( int parent )
#else
void DtExt_SetParentWidget( Widget parent )
#endif
{
    dtParentWidget = parent;
}
    
//  ========== DtGetParent ==========
//
//  SYNOPSIS
//  Return the top level widget. Useful for DSO writers
//  who wish to post a dialog. Use the returned widget
//  as the parent of the dialog.
//

#ifdef WIN32
int  DtGetParent( int *parent )
#else
int  DtGetParent(Widget *parent)
#endif
{

//    *parent = ApMainWindowWidget();

    *parent = dtParentWidget;

    return(1);

}  // DtGetParent //

typedef struct {
    float    translate[3];
    float    scale[3];
    float    rotation[3];     /* Euler angles    */
    float    quaternion[4];   /* quaternion      */
    float    rotMatrix[3][3]; /* rotation matrix */
} DECOMP_MAT;


static void utlMtx2Euler(int ord, float m[3][3], float rot[3]);
static void utlMtx2Quat(float m[3][3], float quat[4]);

#define UTL_ROT_XYZ          0
#define UTL_ROT_XZY          1
#define UTL_ROT_YXZ          2
#define UTL_ROT_YZX          3
#define UTL_ROT_ZXY          4
#define UTL_ROT_ZYX          5

#define XROT                 'x'
#define YROT                 'y'
#define ZROT                 'z'

//  ========== utlDecompMatrix ==========
//
//  SYNOPSIS
//      Decompose a matrix to it's components, translate,
//      rotate ( a quaternion) and scale.
//
static void utlDecompMatrix( const float *mat, DECOMP_MAT *dmat, 
    char *rotOrder)
{
    int         i, j,
        order;
    static float       Sxy, Sxz,
        rot[3], quat[4],
        det,
        m[3][3];

    dmat->translate[0] = mat[3*4+0];
    dmat->translate[1] = mat[3*4+1];
    dmat->translate[2] = mat[3*4+2];

    m[0][0] = mat[0*4+0];
    m[0][1] = mat[0*4+1];
    m[0][2] = mat[0*4+2];

    dmat->scale[0] = (float)sqrt( m[0][0]*m[0][0] + m[0][1]*m[0][1] + 
        m[0][2] * m[0][2]);

    /* Normalize second row */
    m[0][0] /= dmat->scale[0];
    m[0][1] /= dmat->scale[0];
    m[0][2] /= dmat->scale[0];

    /* Determine xy shear */
    Sxy = mat[0*4+0] * mat[1*4+0] + 
        mat[0*4+1] * mat[1*4+1] +
        mat[0*4+2] * mat[1*4+2];

    m[1][0] = mat[1*4+0] - Sxy * mat[0*4+0];
    m[1][1] = mat[1*4+1] - Sxy * mat[0*4+1];
    m[1][2] = mat[1*4+2] - Sxy * mat[0*4+2];

    dmat->scale[1] = (float)sqrt( m[1][0]*m[1][0] + m[1][1]*m[1][1] + 
        m[1][2] * m[1][2]);

    /* Normalize second row */
    m[1][0] /= dmat->scale[1];
    m[1][1] /= dmat->scale[1];
    m[1][2] /= dmat->scale[1];

    /* Determine xz shear */
    Sxz = mat[0*4+0] * mat[2*4+0] + 
        mat[0*4+1] * mat[2*4+1] +
        mat[0*4+2] * mat[2*4+2];

    m[2][0] = mat[2*4+0] - Sxz * mat[0*4+0];
    m[2][1] = mat[2*4+1] - Sxz * mat[0*4+1];
    m[2][2] = mat[2*4+2] - Sxz * mat[0*4+2];

    dmat->scale[2] = (float)sqrt( m[2][0]*m[2][0] + m[2][1]*m[2][1] + 
        m[2][2] * m[2][2]);

    /* Normalize third row */
    m[2][0] /= dmat->scale[2];
    m[2][1] /= dmat->scale[2];
    m[2][2] /= dmat->scale[2];

    det = (m[0][0]*m[1][1]*m[2][2]) + (m[0][1]*m[1][2]*m[2][0]) + 
        (m[0][2]*m[1][0]*m[2][1]) - (m[0][2]*m[1][1]*m[2][0]) - 
        (m[0][0]*m[1][2]*m[2][1]) - (m[0][1]*m[1][0]*m[2][2]);

    /* If the determinant of the rotation matrix is negative, */
    /* negate the matrix and scale factors.                   */
    
    if ( det < 0.0) {
        for ( i = 0; i < 3; i++) {
            for ( j = 0; j < 3; j++) 
                m[i][j] *= -1.0;
            dmat->scale[i] *= -1.0;
        }
    }

    // Copy the 3x3 rotation matrix into the decomposition 
    // structure.
    //
#if _MSC_VER >= 1400
    unsigned int uiSize = sizeof( float)*9;
    int iRet = memcpy_s( dmat->rotMatrix, uiSize, m, uiSize );
    NIASSERT(iRet == 0);
#else // #if _MSC_VER >= 1400
    memcpy( dmat->rotMatrix, m, sizeof( float)*9);
#endif // #if _MSC_VER >= 1400

    rot[1] = (float)asin( -m[0][2]);
    if ( fabsf( (float)cos( rot[1])) > 0.0001) {
        rot[0] = (float)asin( m[1][2]/cos( rot[1]));
        rot[2] = (float)asin( m[0][1]/cos( rot[1]));
    } else {
        rot[0] = (float)acos( m[1][1]);
        rot[2] = 0.0f;
    }

    switch( rotOrder[2]) {
        case XROT:
            if ( rotOrder[1] == YROT) 
                order = UTL_ROT_XYZ;
            else
                order = UTL_ROT_XZY;
            break;

        case YROT:
            if ( rotOrder[1] == XROT) 
                order = UTL_ROT_YXZ;
            else
                order = UTL_ROT_YZX;
            break;

        case ZROT:
            if ( rotOrder[1] == XROT) 
                order = UTL_ROT_ZXY;
            else
                order = UTL_ROT_ZYX;
            break;

        default:
            order = UTL_ROT_XYZ;
            break;
    }

    utlMtx2Euler( order, m, rot);
    dmat->rotation[0] = rot[0];
    dmat->rotation[1] = rot[1];
    dmat->rotation[2] = rot[2];

    utlMtx2Quat(m,quat);
    dmat->quaternion[0] = quat[0];
    dmat->quaternion[1] = quat[1];
    dmat->quaternion[2] = quat[2];
    dmat->quaternion[3] = quat[3];
}


/*
 *  ========== CapQuat2Euler ==========
 *
 *  SYNOPSIS
 *      Convert a quaternion to Euler angles.
 *
 *  PARAMETERS
 *      int                     The order of rotations
 *      float   mat[3][3]       rotation matrix  
 *      float   rot[3]          xyz-rotation values
 *
 *  DESCRIPTION
 *      This routine converts a mateix to Euler angles.
 *      There are a few caveats:
 *          The rotation order for the returned angles is always zyx.
 *      The derivation of this algorithm is taken from Ken Shoemake's
 *      paper:
 *          SIGGRAPH 1985, Vol. 19, # 3, pp. 253-254
 *
 *  RETURN VALUE
 *      None.
 */

#ifdef WIN32
#define M_PI_2 3.14159/2.0
#endif

 static void utlMtx2Euler(int ord, float m[3][3], float rot[3])
{
    /*
     *  Ken Shoemake's recommended algorithm is to convert the
     *  quaternion to a matrix and the matrix to Euler angles.
     *  We do this, of course, without generating unused matrix
     *  elements.
   */
    float        zr, sxr, cxr,
        yr, syr, cyr,
        xr, szr, czr;
    static float epsilon = 1.0e-5f;

    switch ( ord) {

        case UTL_ROT_ZYX:
            syr = -m[0][2];
            cyr = (float)sqrt(1 - syr * syr);

            if (cyr < epsilon) {
                /* Insufficient accuracy, assume that yr = PI/2 && zr = 0 */
                xr = atan2f(-m[2][1], m[1][1]);
                yr = (float)((syr > 0.0f) ? M_PI_2 : -M_PI_2);/* +/- 90 deg */
                zr = 0.0f;
            } else {
                xr = atan2f(m[1][2], m[2][2]);
                yr = atan2f(syr, cyr);
                zr = atan2f(m[0][1], m[0][0]);
            }
            break;

        case UTL_ROT_YZX:
            szr = m[0][1];
            czr = (float)sqrt(1 - szr * szr);
            if (czr < epsilon) {
                /* Insufficient accuracy, assume that zr = +/- PI/2 && yr=0*/
                xr = atan2f(m[1][2], m[2][2]);
                yr = 0.0;
                zr = (float)((szr > 0) ? M_PI_2 : -M_PI_2);
            } else {
                xr = atan2f(-m[2][1], m[1][1]);
                yr = atan2f(-m[0][2], m[0][0]);
                zr = atan2f(szr,  czr);
            }
            break;

        case UTL_ROT_ZXY:
            sxr = m[1][2];
            cxr = (float)sqrt(1 - sxr * sxr);

            if (cxr < epsilon) {
                /* Insufficient accuracy, assume that xr = PI/2 && zr = 0 */
                xr = (float)((sxr > 0) ? M_PI_2 : -M_PI_2);
                yr = atan2f(m[2][0], m[0][0]);
                zr = 0.0;
            } else {
                xr = atan2f( sxr, cxr);
                yr = atan2f(-m[0][2], m[2][2]);
                zr = atan2f(-m[1][0], m[1][1]);
            }
            break;

        case UTL_ROT_XZY:
            szr = -m[1][0];
            czr = (float)sqrt(1 - szr * szr);
            if (czr < epsilon) {
                /* Insufficient accuracy, assume that zr = PI / 2 && xr = 0 */
                xr = 0.0;
                yr = atan2f(-m[0][2], m[2][2]);
                zr = (float)((szr > 0) ? M_PI_2 : -M_PI_2);
            } else {
                xr = atan2f(m[0][2], m[1][1]);
                yr = atan2f(m[2][0], m[0][0]);
                zr = atan2f(szr, czr);
            }
            break;

        case UTL_ROT_YXZ:
            sxr = -m[2][1];
            cxr = (float)sqrt(1 - sxr * sxr);

            if (cxr < epsilon) {
                /* Insufficient accuracy, assume that xr = PI/2 && yr = 0 */
                xr = (float)((sxr > 0) ? M_PI_2 : -M_PI_2);
                yr = 0.0;
                zr = atan2f(-m[1][0], m[0][0]);
            } else {
                xr = atan2f(sxr, cxr);
                yr = atan2f(m[2][0], m[2][2]);
                zr = atan2f(m[0][1], m[1][1]);
            }
            break;

        case UTL_ROT_XYZ:
            syr = m[2][0];
            cyr = (float)sqrt(1 - syr * syr);
            if (cyr < epsilon) {
                /* Insufficient accuracy, assume that yr = PI / 2 && xr = 0 */
                xr = 0.0;
                yr = (float)((syr > 0) ? M_PI_2 : -M_PI_2);
                zr = atan2f(m[0][1], m[1][1]);
            } else {
                xr = atan2f(-m[2][1], m[2][2]);
                yr = atan2f( syr, cyr);
                zr = atan2f(-m[1][1], m[0][0]);
            }
            break;
    }

    rot[0] = xr;
    rot[1] = yr;
    rot[2] = zr;
}

/*
 *  ========= utlMtx2Quat ====================
 * 
 *  SYNOPSIS
 *  Returns the w,x,y,z coordinates of the quaternion
 *  given the rotation matrix.
 */
static void utlMtx2Quat(float m[3][3], float quat[4])
{
    // m stores the 3x3 rotation matrix.
    // Convert it to quaternion.
    float trace = m[0][0] + m[1][1] + m[2][2];
    float s;
    if (trace > 0.0) {
        s = (float)sqrt(trace + 1.0);
        quat[0] = s*0.5f;
        s = 0.5f/s;
        quat[1] = (m[1][2] - m[2][1])*s;
        quat[2] = (m[2][0] - m[0][2])*s;
        quat[3] = (m[0][1] - m[1][0])*s;

    }
    else {
        int i = 0; 
        // i represents index of quaternion, so 0=scalar, 1=xaxis, etc.
        int nxt[3] = {1,2,0}; // next index for each component.
        if (m[1][1] > m[0][0]) i = 1;
        if (m[2][2] > m[i][i]) i = 2;
        int j = nxt[i]; int k = nxt[j];
        s = (float)sqrt( (m[i][i] - (m[j][j] + m[k][k])) + 1.0);
        float q[4];
        q[i+1] = s*0.5f;
        s=0.5f/s;
        q[0] = (m[j][k] - m[k][j])*s;
        q[j+1] = (m[i][j]+m[j][i])*s;
        q[k+1] = (m[i][k]+m[k][i])*s;
        quat[0] = q[0];
        quat[1] = q[1];
        quat[2] = q[2];
        quat[3] = q[3];
    }
}

/*
 *  ========== DtMatrixGetTranslation ==========
 *
 *  SYNOPSIS
 *  Return the x,y,z translation components of the
 *  given matrix. The priority order is assumed to be ---.
 */

int  DtMatrixGetTranslation( float *matrix, float *xTrans, float *yTrans, 
    float *zTrans)
{
    DECOMP_MAT dmat;
    if (matrix)
    {
        utlDecompMatrix( matrix, &dmat, "xyz" );
        *xTrans = dmat.translate[0];
        *yTrans = dmat.translate[1];
        *zTrans = dmat.translate[2];
    }
    else
    {
        *xTrans = *yTrans = *zTrans = 0.0;
    }
    return(1);

}  /* DtMatrixGetTranslation */

/*
 *  ========== DtMatrixGetQuaternion ==========
 *
 *  SYNOPSIS
 *  Return the quaternion (scalar, xAxis, yAxis, zAxis)
 *  defining the orientation represented in the given matrix.
 */
int  DtMatrixGetQuaternion(float *matrix, float *scalar, float *xAxis, 
    float *yAxis, float *zAxis)
{
    DECOMP_MAT dmat;
    if (matrix)
    {
        utlDecompMatrix( matrix, &dmat, "xyz" );
        *scalar = dmat.quaternion[0];
        *xAxis = dmat.quaternion[1];
        *yAxis = dmat.quaternion[2];
        *zAxis = dmat.quaternion[3];
    } 
    else
    {
        *scalar = 1.0; *xAxis = *yAxis = *zAxis = 0.0;
    }
    return(1);
}  /* DtMatrixGetQuaternion */

/*
 *  ========== DtMatrixGetRotation ==========
 *
 *  SYNOPSIS
 *  Return the x,y,z rotation components of the
 *  given matrix. The priority order is assumed to be ---.
 */

int  DtMatrixGetRotation(float *matrix, float *xRotation, float *yRotation, 
    float *zRotation)
{

    DECOMP_MAT dmat;
    if (matrix)
    {
        utlDecompMatrix( matrix, &dmat, "xyz" );
        *xRotation = dmat.rotation[0];
        *yRotation = dmat.rotation[1];
        *zRotation = dmat.rotation[2];
    }
    else
    {
        *xRotation = *yRotation = *zRotation = 0.0;
    }
    return(1);

}  /* DtMatrixGetRotation */


/*
 *  ========== DtMatrixGetScale ==========
 *
 *  SYNOPSIS
 *  Return the x,y,z scale components of the given
 *  matrix. The priority order is assumed to be ---.
 */

int  DtMatrixGetScale(float *matrix, float *xScale, float *yScale, 
    float *zScale)
{

    DECOMP_MAT dmat;
    
    if (matrix)
    {
        utlDecompMatrix( matrix, &dmat, "xyz" );
        *xScale = dmat.scale[0];
        *yScale = dmat.scale[1];
        *zScale = dmat.scale[2];
    }
    else
    {
        *xScale = *yScale = *zScale = 1.0;
    }
    return(1);

}  /* DtMatrixGetScale */
/*
 *  ========== DtMatrixGetTransforms ==========
 *
 *  SYNOPSIS
 *  Return the x,y,z translation, scale quaternion and
 *  Euler angles in "xyz" order of the given
 *  matrix. 
 */

int DtMatrixGetTransforms(float *matrix, float *translate, 
                          float *scale, float *quaternion, float *rotation)
{
    DECOMP_MAT dmat;

    if (matrix)
    {
        utlDecompMatrix( matrix, &dmat, "xyz" );

    if (translate) {
        translate[0] = dmat.translate[0];
        translate[1] = dmat.translate[1];
        translate[2] = dmat.translate[2];
    }
    if (scale) {
        scale[0] = dmat.scale[0];
        scale[1] = dmat.scale[1];
        scale[2] = dmat.scale[2];
    }
    if (quaternion) {
        quaternion[0] = dmat.quaternion[0];
        quaternion[1] = dmat.quaternion[1];
        quaternion[2] = dmat.quaternion[2];
        quaternion[3] = dmat.quaternion[3];
    }
    if (rotation) {
        rotation[0] = dmat.rotation[0];
        rotation[1] = dmat.rotation[1];
        rotation[2] = dmat.rotation[2];
    }
        return(1);
    }
    return(0);
}

//  ========== DtImageWrite ==========
//
//  SYNOPSIS
//  Write out an image file.
//
#if 0 
int  DtImageWrite(char *name, char *key, int width, int height, 
    int components, unsigned char *image, IMF_CAP_SETTING **settings, 
    IMF_LUT *lut)
{
    ImfInfo imf_info;
    char    cbuf[MAXPATHLEN];
    char    *outd;
    int     type, ret;
    int     err = 0;


    // set number of components in image
    //
    switch (components)
    {
        case 1 :
            type = IMG_INDEX;
            break;

        case 3 :
            type = IMG_RGB;
            break;

        case 4 :
            type = IMG_RGBA;
            break;

        default :
            fprintf(stderr, "ERROR - unsupported image format: %d (%s:%d)\n",
                components, __FILE__, __LINE__);
            return(0);
    }


    // create the ImfInfo structure for image header info
    //
    imf_info = ImfInfoAlloc();


    // set program name and description
    //
    NiSprintf(cbuf, MAXPATHLEN, "%s %s", AP_PGM_NAME, AP_PGM_VERSION);
    ImfInfoProgram(imf_info, cbuf);
    ImfInfoDescription(imf_info, "Exported texture.");


    // set the image type and format
    //
    ImfInfoKey(imf_info, key);
    ImfInfoFormat(imf_info, "unknown", width, height, 1.0);

    // set the image output filename
    //
    DtGetDirectory(&outd);
    ImfInfoFilename(imf_info, outd, name);


    // set the image create time and no frame number
    //
    ImfInfoTime(imf_info, NULL);
    ImfInfoFrame(imf_info, 0);

    // set the image capabilities.
    //
    ImfInfoSettings( imf_info, settings);

    // set the image look up table.
    //
    ImfInfoLut( imf_info, lut);

    // write image to disk
    //
    ret = ImfImageWrite(imf_info, type, (void *)image);

    if (ret == 0)
    {
        NiSprintf(cbuf, MAXPATHLEN,
            "Unable to write texture to disk:\n\n%s/%s", 
            outd, name);
        /* UtErrorDialog(ApMainWindowWidget(), cbuf); */
        DtExt_Err( cbuf );
        err = 1;
    }


    // free ImfInfo structure
    //
    ImfInfoFree(imf_info);

    // return return error status
    //
    return(err ? 0 : 1);

}  // DtImageWrite //

//  ========== DtImageRead ==========
//
//  SYNOPSIS
//  Read an image and return the rgb data.
//
void DtImageRead( char *name, int *width, int *height, int *components, 
    unsigned char **image )
{
    int                  frame = 1;
    char                 resultName[_MAX_PATH];
    IMF_INFO             texInfo;
    IMF_OBJECT_INFO     *imfInfo;
    char                *buf;
    IMF_OBJECT          *texObj = NULL;
    IMF_COLOR_RESPONSE   outputGamma;



    *image = NULL;
    *width = *height = 0;
    *components = 3;

    // initialize the image processing library
    //
    if ( IMF_init( NULL, NULL) != IMF_C_SUCCESS)
        return;

    IMF_info_init( &texInfo );

    texInfo.key    = NULL;
    texInfo.handle = NiStrdup( name );

    // added this because IMF_open() is broken
    //
    IMF_pathname_build_no_name_syntax( name, NULL, IMF_PATHNAME_FILENAME, 
        FALSE, &frame, resultName, NULL, &texInfo.key, &imfInfo);

    // Open the texture.
    //
    if ( !(texObj = IMF_open( &texInfo, "r")) )
        return;

    IMF__select( texObj, 0);

    outputGamma.usage = IMF_C_CORRECTION_GAMMA;
    outputGamma.gamma = (buf = getenv( "WF_GAMMA_VALUE")) ? atof( buf) : 1.7;

    texObj = IMF__display( texObj, IMF_C_CHAN_RED | IMF_C_CHAN_GREEN | 
        IMF_C_CHAN_BLUE, NULL, &outputGamma);

    // Determine the texture resolution.
    //
    *width  = texObj->info.image[0].window.right + 1;
    *height = texObj->info.image[0].window.top   + 1;

    // Check the orientation of the image.
    //
    int start = 0;
    int limit = *height;
    int incr  = 1;

    if ( texObj->info.image[0].chan_orient & IMF_C_ORIENT_TOP_LEFT )
    {
        start   = *height - 1;
        limit   = -1;
        incr    = -1;
    }

    // read each scanline of the image to build texture map
    //
    if ( *image = ( unsigned char *) NiMalloc ( sizeof( unsigned char) * 
        (*width)* (*height) * 3))
    {
        for ( int i = start; i != limit; i += incr)
        {
            unsigned char **rgb;

            // read a scanline
            //
            if ((*texObj->scan)(texObj->data[0], i, &rgb) != IMF_C_NORMAL)
                break;

            for ( int j = 0; j < *width; j++)
            {
                int offset = (i * *width * 3) + ( j * 3 );

                for ( int k = 0; k < 3; k++)
                {
                    (*image)[ offset+k] = rgb[k][j];
                }
            }
        }
    }

    // close the imf object
    //
    (*texObj->close)( texObj );
}

#endif

/*
 *  ========== DtSetFilename ==========
 *
 *  SYNOPSIS
 *  Set the output base filename.
 */

int  DtSetFilename(char *name)
{

    if (out_name != NULL) NiFree(out_name);
    out_name = NiStrdup(name);

    return(1);

}  /* DtSetFilename */


/*
 *  ========== DtSetDirectory ==========
 *
 *  SYNOPSIS
 *  Set the output directory.
 */

int DtSetDirectory( char *directory )
{
    if( NULL != out_dir ) 
    {
        NiFree(out_dir);
    }
    out_dir = NiStrdup( directory ); // where is out_dir freed?

    return(1);

}  /* DtSetDirectory */


//
// ===========================================================
// ===================== S H A P E ===========================
// ===========================================================


/*
 *  ========== DtEnvGetAttenuation ==========
 *
 *  SYNOPSIS
 *  Returns the global attenuation for the scene. The
 *  parameters a2, a1, and a0 are the squared, linear,
 *  and constant cooeffiecents.
 */

void  DtEnvGetAttenuation(float *a2, float *a1, float *a0)
{

    *a2 = 0.0;
    *a1 = 0.0;
    *a0 = 0.0;
    return;

}  /* DtEnvGetAttenuation */

/*
 *  ========== DtEnvGetFogType ==========
 *
 *  SYNOPSIS
 *  Return the global fog type for the scene.
 */

int  DtEnvGetFogType(int *type)
{

    *type = 0;
    return(1);

}  /* DtEnvGetFogType */


/*
 *  ========== DtEnvGetFogColor ==========
 *
 *  SYNOPSIS
 *  Return the fog color.
 */

void  DtEnvGetFogColor(float *red, float *green, float *blue)
{

    *red = 0.0;
    *green = 0.0;
    *blue = 0.0;
    return;

}  /* DtEnvGetFogColor */


/*
 *  ========== DtEnvGetFogVisibility ==========
 *
 *  SYNOPSIS
 *  Return the distance at which fot totally obscures the
 *  objects in the scene.
 */

int  DtEnvGetFogVisibility(float *visibility)
{

    *visibility = 0.0;
    return(1);

}  /* DtEnvGetFogVisibility */



// ========================================================================
// -------------------  KIN Stub PUBLIC  FUNCTIONS  -----------------------
// ========================================================================

//
//  As this implementation is for Alias wire files OpenAlias/Model API
//  We really don't have the "kin" functions to worry about.
//  So that there are no problems with linking and calling unresolved
//  functions let's put in these stubs
//
//  Remember to check the return status from functions
//

//  ========== DtKinGetBodyCount ==========
//
//  SYNOPSIS
//  Returns the number of bodies in the scene.
//

int  DtKinGetBodyCount (int *count)
{
    // return the body count
    //
    *count = 0;
    return(1);


}  // DtKinGetBodyCount //



//  ========== DtKinGetBodyName ==========
//
//  SYNOPSIS
//  Returns a body name.
//

int  DtKinGetBodyName (int /* bodyID */, char **name)
{
    // validate the bodyID
    //
    *name = NULL;
    return(0);

}  // DtKinGetBodyName //



//  ========== DtKinGetSegmentCount ==========
//
//  SYNOPSIS
//  Return the number of segments in the body.
//

int  DtKinGetSegmentCount (int /* bodyID */, int *count)
{
    *count = 0;
    return(0);

}  // DtKinGetSegmentCount //



//  ========== DtKinGetSegmentName ==========
//
//  SYNOPSIS
//  Returns a segment name.
//

int  DtKinGetSegmentName (int /* bodyID */, int /* segID */, char **name)
{

    // validate the bodyID and segID
    //
    *name = NULL;
    return(0);

}  // DtKinGetSegmentName //



//  ========== DtKinGetSegmentXfm ==========
//
//  SYNOPSIS
//  Returns a segment global transformation for the given frame.
//

int  DtKinGetSegmentXfm (int /* bodyID */, int /* segID */, int /* frame */, 
    float **xfm)
{
    // validate the bodyID and segID
    //
    *xfm = NULL;
    return(0);

}  // DtKinGetSegmentXfm //



//  ========== DtKinGetGroupCount ==========
//
//  SYNOPSIS
//  Returns the number of point groups in the segment.
//

int  DtKinGetGroupCount (int /* bodyID */, int /* segID */, int *count)
{
    // validate the bodyID and segID
    //
    *count = 0;
    return(0);

}  // DtKinGetGroupCount //



//  ========== DtKinGetGroupName ==========
//
//  SYNOPSIS
//  Returns the point group name.
//

int  DtKinGetGroupName (int /* bodyID */, int /* segID */, int /* groupID */, 
    char **name)
{
    // validate the bodyID and segID and groupID
    //
    *name = NULL;
    return(0);

}  // DtKinGetGroupName //



/* ======================================================================= *
 * --------------------  PRIVATE FUNCTIONS  ------------------------------ *
 * ======================================================================= */


/*
 *  ========== sceneNew ==========
 *
 *  SYNOPSIS
 *  A private function. Reset the internal states of this file.
 */

void  sceneNew(void)
{

    // Create the object instance structure.
    //
    if( NULL == local ) 
    {
        local = (DtPrivate *)NiAlloc(DtPrivate, 1);
        memset(local, 0, sizeof(DtPrivate));
    }

    // Clear the old scene name.
    //
    if( NULL != local->name )
    {
        NiFree( local->name );
        local->name = NULL;
    }

    // Clear old key frame array.
    //
    if( NULL != local->keyFrames )
    {
        NiFree( local->keyFrames );
        local->keyCount = 0;
    }

    // Fill in the scene
    // 
    sceneCreateCaches();

}  /* sceneNew */


/*
 *  ========== sceneCreateCaches ==========
 *
 *  SYNOPSIS
 *  Create the node caches and prepare for exporting.
 */

void  sceneCreateCaches(void)
{
    local->frame_start  = kFrameStartDefault;
    local->frame_end    = kFrameEndDefault;
    local->frame_by     = kFrameByDefault;
    local->start        = kFrameStartDefault;
    local->end          = kFrameEndDefault;
    local->by           = kFrameByDefault;

    // For now the keyframe support is not implemented (from Alias DT): 
    //
#if 0
    // Collect the user key frames and build a local
    // array for them.
    //
    int i, count;

    for( i = 0, count = 0; i < app->key_ct; i++ )
    {
        // Count how many key frames there are.
        //
        if( 0 != app->keys[i]) 
        {
            count++;
        }
    }

    // Create the local array for the key frames.
    //
    local->keyCount = count;
    local->keyFrames = ( count) ? (int *)NiMalloc( count * sizeof( int ) ) : 
        NULL;

    // Fill the array with actual key frame values.
    //
    for( i = 0, count = 0; i < app->key_ct; i++ )
    {
        if( 0 != app->keys[i]) 
        {
            local->keyFrames[count++] = local->frame_start + i;
        }
    }
#endif
}  // sceneCreateCaches //


//  ========== sceneSetName ==========
//
//  SYNOPSIS
//  Set the scene name.
//
void  sceneSetName(char *name)
{

    if( NULL == local ) 
    {
        return;
    }

    // Replace old name with new name.
    //
    if( NULL != local->name ) 
    {
        NiFree( local->name );
    }
    if( NULL != name ) 
    {
        local->name = NiStrdup( name );
    }
    return;
}  // sceneSetName //


//  ========== DtExt_firstDagNode ==========
//
//  SYNOPSIS
//  Return the first DAG node as seen by OpenModel. 
//

#if 0
void DtExt_firstDagNode( AlObject** dest )
{
    AlDagNode* top;

    if ( dest && *dest )
    {
        if ( top = AlUniverse::firstDagNode() )
        {
            *dest = (AlObject*) top;
        }
        else
        {
            *dest = NULL;
        }
    }
} // DtExt_firstDagNode //
#endif


#ifdef WIN32
#pragma warning(default: 4244 4305)
#endif // WIN32


