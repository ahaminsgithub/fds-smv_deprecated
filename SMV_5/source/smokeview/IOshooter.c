// $Date$ 
// $Revision$
// $Author$

#include "options.h"
#ifdef pp_SHOOTER
#include <string.h>
#ifdef pp_OSX
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include "flowfiles.h"
#include "ASSERT.h"
#include "MALLOC.h"
#include "smokeviewdefs.h"
#include "smokeviewvars.h"
#include "smokeheaders.h"

// svn revision character string
char IOshooter_revision[]="$Revision$";
// $Date$ $Author$

/* ------------------ allocate_shooter ------------------------ */

int  allocate_shooter(void){
  int mem_points, mem_frames;

  FREEMEMORY(shootpointinfo);
  FREEMEMORY(shoottimeinfo);

  mem_points=max_shooter_points*sizeof(shootpointdata);
  mem_frames=nshooter_frames*sizeof(shoottimedata);

  printf("shooter point memory requirements\n");
  printf("max_shooter_points=%i mem=%i\n",max_shooter_points,mem_points);
  printf("nshooter_frames=%i mem=%i\n",nshooter_frames,mem_frames);

  if(  mem_points<=0||mem_frames<=0||
#ifdef _DEBUG
       mem_points>=2000000000||mem_frames>2000000000||
#endif
    NewMemory((void **)&shootpointinfo,mem_points)==0||
    NewMemory((void **)&shoottimeinfo,mem_frames)==0){
    FREEMEMORY(shootpointinfo);
    FREEMEMORY(shoottimeinfo);
    shooter_active=0;
    printf("shooter point memory allocation failed\n");
    return 1;
  }
  return 0;

}

/*
typedef struct _shootpointdata {
  struct _shootpointdata *prev;
  float xyz[3], uvw[3], val;
} shootpointdata;

typedef struct {
  float time;
  int frame;
  shootpointdata *beg, *end;
} shoottimedata;
*/

/* ------------------ get_vel ------------------------ */

void get_shooter_vel(float *uvw, float *xyz){
  float factor;

  factor = pow(xyz[2]/shooter_z0,shooter_p);
  uvw[0] = factor*shooter_velx;
  uvw[1] = factor*shooter_vely;
  uvw[2] = shooter_velz;
}

/* ------------------ increment_shooter_data ------------------------ */

void increment_shooter_data(float dt){
  int i;
  float *xyz, *uvw, uvw_air[3];
  float g=9.8;
  
  // dv/dt = g - |g|(v-v_a)/v_inf
  // dx/dt = v

  shooter_time+=dt;
  for(i=0;i<shooter_nparts;i++){
    float dvel[3];

    xyz = shootpointinfo[i].xyz;
    uvw = shootpointinfo[i].uvw;
    get_shooter_vel(uvw_air,xyz);

    dvel[0] = uvw[0]-uvw_air[0];
    dvel[1] = uvw[1]-uvw_air[1];
    dvel[2] = uvw[2]-uvw_air[2];

    uvw[0] -= g*dt*(    dvel[0]/shooter_v_inf);
    uvw[1] -= g*dt*(    dvel[1]/shooter_v_inf);
    uvw[2] -= g*dt*(1.0+dvel[2]/shooter_v_inf);

    xyz[0] += dt*uvw[0];
    xyz[1] += dt*uvw[1];
    xyz[2] += dt*uvw[2];
  }
  shooter_active=1;
}

/* ------------------ init_shooter_data ------------------------ */

void init_shooter_data(void){
  int i;
  float *xyz, *uvw;
  float xmin, ymin, zmin;

  xmin = shooter_xyz[0]-shooter_dxyz[0]/2.0;
  ymin = shooter_xyz[1]-shooter_dxyz[1]/2.0;
  zmin = shooter_xyz[2]-shooter_dxyz[2]/2.0;
  //shooter_firstpass=1;
//  show_shooter=0;
  
  for(i=0;i<shooter_nparts;i++){
    xyz = shootpointinfo[i].xyz;
    uvw = shootpointinfo[i].uvw;
    xyz[0] = xmin + shooter_dxyz[0]*(float)rand()/RAND_MAX;
    xyz[1] = ymin + shooter_dxyz[1]*(float)rand()/RAND_MAX;
    xyz[2] = zmin + shooter_dxyz[2]*(float)rand()/RAND_MAX;
    uvw[0]=0.0;
    uvw[1]=0.0;
    uvw[2]=0.0;
    shootpointinfo[i].prev=NULL;
    shootpointinfo[i].val=0.0;
  }
  shoottimeinfo[0].beg=shootpointinfo;
  shoottimeinfo[0].end=shootpointinfo+shooter_nparts-1;
  shoottimeinfo[0].frame=0;
  shoottimeinfo[0].time=0.0;
  shooter_active=1;
  shooter_time=0.0;
}

/* ------------------ draw_shooter ------------------------ */

void draw_shooter(void){
  int i;


  increment_shooter_data(0.01);
  if(shooter_time>shooter_time_max){
    printf("initializing shooter data\n");
    init_shooter_data();
  }
  glPointSize(shooterpointsize);

  glPushMatrix();
  glScalef(1.0/xyzmaxdiff,1.0/xyzmaxdiff,1.0/xyzmaxdiff);
  glTranslatef(-xbar0,-ybar0,-zbar0);

  //glColor4fv(static_color);
  glBegin(GL_POINTS);
  for(i=0;i<shooter_nparts;i++){
    float *xyz;

    xyz = shootpointinfo[i].xyz;
    glVertex3fv(xyz);
  }
  glEnd();
  glPopMatrix();
}


#endif
