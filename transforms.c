#include "decs.h"

// this file includes all coordinate transformations and velocity transformations
// No user functions, unless new transformation of coordinates required


// NOTE: When dealing with multiple coordinate geom structures, must keep the PRIMECOORD one separate since repoint pointer to storage of PRIMECOORD geom structure, and using that pointer again afterwards for non-PRIMECOORD or other uses would overwrite PRIMECOORD geom structure values used during evolution and elsewhere when get_geometry() called.


// assumes all centered quantities (so for FLUXB==FLUXCTSTAG assumes operates on centered field versions)
int bl2met2metp2v(int whichvel, int whichcoord, FTYPE *pr, int ii, int jj, int kk)
{
  int loc;

  loc=CENT;
 
  return(bl2met2metp2v_genloc(whichvel, whichcoord, pr, ii, jj, kk, loc));

}


// converts whichvel/whichcoord velocity to WHICHVEL/(->MCOORD->PRIMECOORDS)
// converts field too
int bl2met2metp2v_genloc(int whichvel, int whichcoord, FTYPE *pr, int ii, int jj, int kk, int loc)
{
  int k = 0;
  FTYPE ucon[NDIM];
  struct of_geom geomdontusebl;
  struct of_geom *ptrgeombl=&geomdontusebl;
  struct of_geom geomdontuse;
  struct of_geom *ptrgeom=&geomdontuse;
  FTYPE Bcon[NDIM];

  // whichvel==0 means supplied the 4-velocity
  // whichvel==1 means supplied the 3-velocity
  // whichvel==2 means supplied the relative 4-velocity

  // if whichcoord==PRIMECOORDS, then really use uses pr2ucon and ucon2pr, could probably optimize if wanted
  // effectively this results in changing from one primitive velocity to another within PRIMECOORDS


  // pr is in whichcoord coordinates
  // get geometry (non-prime coords)
  gset_genloc(0,whichcoord,ii,jj,kk,loc,ptrgeombl);
  // convert whichvel-pr in whichcoord coords to ucon in whichcoord coordinates
  if (pr2ucon(whichvel,pr, ptrgeombl ,ucon) >= 1) FAILSTATEMENT("transforms.c:bl2met2metp2v_genloc()", "pr2ucon()", 1);

  // convert field
  Bcon[0]=0.0;
  SLOOPA(k) Bcon[k]=pr[B1+k-1];

  // convert from whichcoord to MCOORD, the coordinates of evolution
  if(whichcoord>=0){
    coordtrans(whichcoord,MCOORD,ii,jj,kk,loc,ucon);
    // transform MCOORD ucon from MCOORD non-prime to MCOORD prime coords
    mettometp_genloc(ii,jj,kk,loc,ucon);

    // field
    coordtrans(whichcoord,MCOORD,ii,jj,kk,loc,Bcon);
    mettometp_genloc(ii,jj,kk,loc,Bcon);

  }
  // otherwise already in prime

  // get prime geometry
  get_geometry(ii,jj,kk,loc,ptrgeom) ;
  // convert from MCOORD prime 4-vel to MCOORD prime WHICHVEL-vel(i.e. primitive velocity of evolution)
  ucon2pr(WHICHVEL,ucon,ptrgeom,pr);

  // convert field
  SLOOPA(k) pr[B1+k-1]=Bcon[k];

  return(0);
}





// converts u_\mu in whichcoord to PRIMECOORDS
int ucov_whichcoord2primecoords(int whichcoord, int ii, int jj, int kk, int loc, FTYPE *ucov)
{
  int k = 0;
  FTYPE ucon[NDIM];
  struct of_geom geomdontuse;
  struct of_geom *ptrgeom=&geomdontuse;
  struct of_geom geomprimedontuse;
  struct of_geom *ptrgeomprime=&geomprimedontuse;
  FTYPE idxdxp[NDIM][NDIM];

  // pr is in whichcoord coordinates
  // get geometry (non-prime coords)
  gset_genloc(0,whichcoord,ii,jj,kk,loc, ptrgeom);

  // first raise vector in whichcoord coordinates to MCOORD coordinates
  raise_vec(ucov,ptrgeom,ucon);
  coordtrans(whichcoord,MCOORD,ii,jj,kk,loc,ucon);

  // get idxdxp
  idxdxprim_ijk(ii, jj, kk, loc, idxdxp);

  // convert to PRIMECOORDS
  mettometp_simple(idxdxp, ucon);

  // get prime geometry
  get_geometry(ii,jj,kk,loc,ptrgeomprime) ;

  // lower back
  lower_vec(ucon,ptrgeomprime,ucov);

  return(0);
}



// converts whichvel/whichcoord velocity to WHICHVEL/MCOORD
int bl2met2metp2v_gen(int whichvel, int whichcoord, int newwhichvel, int newwhichcoord, FTYPE *pr, int ii, int jj, int kk)
{
  int k = 0;
  FTYPE ucon[NDIM];
  struct of_geom geomdontusebl;
  struct of_geom *ptrgeombl=&geomdontusebl;
  struct of_geom geomdontuse;
  struct of_geom *ptrgeom=&geomdontuse;
  FTYPE Bcon[NDIM];


  // whichvel==0 means supplied the 4-velocity
  // whichvel==1 means supplied the 3-velocity
  // whichvel==2 means supplied the relative 4-velocity

  // if whichcoord==PRIMECOORDS, then really use uses pr2ucon and ucon2pr, could probably optimize if wanted
  // effectively this results in changing from one primitive velocity to another within PRIMECOORDS


  // pr is in whichcoord coordinates
  // get geometry (non-prime coords)
  gset(0,whichcoord,ii,jj,kk,ptrgeombl);
  // convert whichvel-pr in whichcoord coords to ucon in whichcoord coordinates
  if (pr2ucon(whichvel,pr, ptrgeombl ,ucon) >= 1) FAILSTATEMENT("transforms.c:bl2met2metp2v_gen()", "pr2ucon()", 1);


  // field
  Bcon[0]=0.0;
  SLOOPA(k) Bcon[k]=pr[B1+k-1];


  // convert from whichcoord to MCOORD, the coordinates of evolution
  if(whichcoord>=0){
    coordtrans(whichcoord,newwhichcoord,ii,jj,kk,CENT,ucon);
    // transform MCOORD ucon from MCOORD non-prime to MCOORD prime coords
    mettometp(ii,jj,kk,ucon);

    coordtrans(whichcoord,newwhichcoord,ii,jj,kk,CENT,Bcon);
    mettometp(ii,jj,kk,Bcon);

  }
  // otherwise already in prime

  // get prime geometry
  get_geometry(ii,jj,kk,CENT,ptrgeom) ;
  // convert from MCOORD prime 4-vel to MCOORD prime WHICHVEL-vel(i.e. primitive velocity of evolution)
  ucon2pr(newwhichvel,ucon,ptrgeom,pr);

  // convert field
  SLOOPA(k) pr[B1+k-1]=Bcon[k];

  return(0);
}


// transform MCOORD prime primitive velocity to whichcoord whichvel velocity (also converts field)
int metp2met2bl(int whichvel, int whichcoord, FTYPE *pr, int ii, int jj, int kk)
{
  int loc;

  loc=CENT;
 
  return(metp2met2bl_genloc(whichvel, whichcoord, pr, ii, jj, kk, loc));

}

// transform MCOORD prime primitive velocity to whichcoord whichvel velocity (also converts field)
int metp2met2bl_genloc(int whichvel, int whichcoord, FTYPE *pr, int ii, int jj, int kk, int pos)
{
  int k;
  FTYPE ucon[NDIM];
  struct of_geom geomdontuse;
  struct of_geom *ptrgeom=&geomdontuse;
  struct of_geom geomdontusebl;
  struct of_geom *ptrgeombl=&geomdontusebl;
  FTYPE Bcon[NDIM];



  // which=WHICHVEL
  // which==0 means supplied the 4-velocity
  // which==1 means supplied the 3-velocity
  // which==2 means supplied the relative 4-velocity

  // if whichcood==PRIMECOORDS, then just pr2ucon and ucon2pr
  // effectively this results in changing from one primitive velocity to another within PRIMECOORDS


  // get prime MCOORD geometry
  get_geometry(ii,jj,kk,pos,ptrgeom) ;
  // transform prime MCOORD primitive to prim MCOORD 4-vel
  //  if (pr2ucon(WHICHVEL,pr, ptrgeom ,ucon) >= 1) FAILSTATEMENT("transforms.c:metp2met2bl()", "pr2ucon()", 1);
  MYFUN(pr2ucon(WHICHVEL,pr, ptrgeom ,ucon),"transforms.c:pr2ucon()","metp2met2bl",0);

  // field
  Bcon[0]=0.0;
  SLOOPA(k) Bcon[k]=pr[B1+k-1];



  if(whichcoord>=0){
    // transform from prime MCOORD 4-vel to non-prime MCOORD 4-vel
    metptomet_genloc(ii,jj,kk,pos,ucon);
    // transform from non-prime MCOORD to non-prime whichcoord
    coordtrans(MCOORD,whichcoord,ii,jj,kk,pos,ucon);

    // convert field
    metptomet_genloc(ii,jj,kk,pos,Bcon);
    coordtrans(MCOORD,whichcoord,ii,jj,kk,pos,Bcon);

  }
  // else already in prime


  // transform from non-prime whichcoord 4-vel to non-prime whichcoord whichvel-velocity
  // can't use same ptrgeom here, since ptrgeom would then the PRIMECOORD version would be overwritten by "whichcoord" version
  gset_genloc(0,whichcoord,ii,jj,kk,pos,ptrgeombl);

  ucon2pr(whichvel,ucon,ptrgeombl,pr);

  // convert field
  SLOOPA(k) pr[B1+k-1]=Bcon[k];

  return(0);
}





// whichcoordin -> whichcoordout
int coordtrans(int whichcoordin, int whichcoordout, int ii, int jj, int kk, int loc, FTYPE *ucon)
{
  // GODMARK: need transformation from BL to KS_JP1 for EP3!=0 or THETAROT!=0
  if(whichcoordin==whichcoordout){// then no transformation
    return(0);
  }
  else if((whichcoordin==BLCOORDS)&&(whichcoordout==KSCOORDS||whichcoordout==KS_JP1_COORDS)){
    bltoks(ii,jj,kk ,loc,ucon);    
  }
  else if((whichcoordin==KSCOORDS || whichcoordin==KS_JP1_COORDS)&&(whichcoordout==BLCOORDS)){
    kstobl(ii,jj,kk ,loc,ucon);    
  }
  else{
    dualfprintf(fail_file,"HARDFAIL: No such transformation: %d -> %d\n",whichcoordin,whichcoordout);
    myexit(1);
  }

  return(0);

}

  
/* transforms u^i to our ks from boyer-lindquist */
void bltoks(int ii, int jj, int kk, int loc, FTYPE*ucon)
{
  FTYPE tmp[NDIM];
  FTYPE trans[NDIM][NDIM];
  FTYPE V[NDIM], r, th;
  int j,k;


  bl_coord_ijk(ii,jj,kk,loc,V) ;
  r=V[1]; th=V[2];

  // don't rotate, because assume bltoks() only called to make local transformation of u^i from BL to KS with same alignment, and notice no angle factors in transformation -- so nothing to transform anyways!
  //  FTYPE Vmetric[NDIM];
  //  rotate_V(BLCOORD,V,Vmetric);
  //  r=Vmetric[1]; th=Vmetric[2];


  // bl2ks for contravariant components
#define bl2ks_trans00   (1)
#define bl2ks_trans01   (2.*r/(r*r - 2.*r + a*a))
#define bl2ks_trans02   (0)
#define bl2ks_trans03   (0)
#define bl2ks_trans10   (0)
#define bl2ks_trans11   (1)
#define bl2ks_trans12   (0)
#define bl2ks_trans13   (0)
#define bl2ks_trans20   (0)
#define bl2ks_trans21   (0)
#define bl2ks_trans22   (1)
#define bl2ks_trans23   (0)
#define bl2ks_trans30   (0)
#define bl2ks_trans31   (a/(r*r - 2.*r + a*a))
#define bl2ks_trans32   (0)
#define bl2ks_trans33   (1)

  /* make transform matrix */
  // order for trans is [ourmetric][bl]
  // DLOOP(j,k) trans[j][k] = 0. ;
  // DLOOPA(j) trans[j][j] = 1. ;
  trans[0][0] = bl2ks_trans00;
  trans[0][1] = bl2ks_trans01;
  trans[0][2] = bl2ks_trans02;
  trans[0][3] = bl2ks_trans03;
  trans[1][0] = bl2ks_trans10;
  trans[1][1] = bl2ks_trans11;
  trans[1][2] = bl2ks_trans12;
  trans[1][3] = bl2ks_trans13;
  trans[2][0] = bl2ks_trans20;
  trans[2][1] = bl2ks_trans21;
  trans[2][2] = bl2ks_trans22;
  trans[2][3] = bl2ks_trans23;
  trans[3][0] = bl2ks_trans30;
  trans[3][1] = bl2ks_trans31;
  trans[3][2] = bl2ks_trans32;
  trans[3][3] = bl2ks_trans33;
  /* transform ucon; solve for v */
  // this is u^j = T^j_k u^k
  DLOOPA(j) tmp[j] = 0.;
  DLOOP(j,k) tmp[j] += trans[j][k] * ucon[k];
  DLOOPA(j) ucon[j] = tmp[j];

  /* done! */
}


/* transforms u^i to our ks from boyer-lindquist */
void kstobl(int ii, int jj, int kk, int loc, FTYPE*ucon)
{
  FTYPE tmp[NDIM];
  FTYPE trans[NDIM][NDIM];
  FTYPE V[NDIM], r, th;
  int j,k;

  bl_coord_ijk(ii,jj,kk,loc,V) ;
  r=V[1]; th=V[2];

  // don't rotate, because assume kstobl() only called to make local transformation of u^i from BL to KS with same alignment, and notice no angle factors in transformation -- so nothing to transform anyways!
  //  FTYPE Vmetric[NDIM];
  //  rotate_V(KSCOORD,V,Vmetric);
  //  r=Vmetric[1]; th=Vmetric[2];


  // just inverse (no transpose) of above
#define ks2bl_trans00   (1)
#define ks2bl_trans01   (-2.*r/(r*r - 2.*r + a*a))
#define ks2bl_trans02   (0)
#define ks2bl_trans03   (0)
#define ks2bl_trans10   (0)
#define ks2bl_trans11   (1)
#define ks2bl_trans12   (0)
#define ks2bl_trans13   (0)
#define ks2bl_trans20   (0)
#define ks2bl_trans21   (0)
#define ks2bl_trans22   (1)
#define ks2bl_trans23   (0)
#define ks2bl_trans30   (0)
#define ks2bl_trans31   (-a/(r*r - 2.*r + a*a))
#define ks2bl_trans32   (0)
#define ks2bl_trans33   (1)



  /* make transform matrix */
  // order for trans is [ourmetric][bl]
  // DLOOP(j,k) trans[j][k] = 0. ;
  // DLOOPA(j) trans[j][j] = 1. ;
  trans[0][0] = ks2bl_trans00;
  trans[0][1] = ks2bl_trans01;
  trans[0][2] = ks2bl_trans02;
  trans[0][3] = ks2bl_trans03;
  trans[1][0] = ks2bl_trans10;
  trans[1][1] = ks2bl_trans11;
  trans[1][2] = ks2bl_trans12;
  trans[1][3] = ks2bl_trans13;
  trans[2][0] = ks2bl_trans20;
  trans[2][1] = ks2bl_trans21;
  trans[2][2] = ks2bl_trans22;
  trans[2][3] = ks2bl_trans23;
  trans[3][0] = ks2bl_trans30;
  trans[3][1] = ks2bl_trans31;
  trans[3][2] = ks2bl_trans32;
  trans[3][3] = ks2bl_trans33;

  /* transform ucon; solve for v */
  // this is u^j = T^j_k u^k
  DLOOPA(j) tmp[j] = 0.;
  DLOOP(j,k) tmp[j] += trans[j][k] * ucon[k];
  DLOOPA(j) ucon[j] = tmp[j];

  /* done! */
}



// transformation of metric written in V[X]-type coordinates with dV differentials to one written in Vmetric old/original coordinates with dVmetric original/old differentials
//
// This is *not* used in metric.c to rotate metric
// This would only be used to transform V[X] metric to old/original Vmetric=rold,hold,phold
void transV2Vmetric(int whichcoord, int ii, int jj, int kk, int loc, FTYPE *X, FTYPE *V, FTYPE *Xmetric, FTYPE *Vmetric, FTYPE*gcov, FTYPE *gcovpert)
{

  if(THETAROT!=0.0 && ALLOWMETRICROT==1 && ISSPCMCOORD(whichcoord)){

#if(0)// if input X,V,Xmetric,Vmetric, no longer need to duplicate doing below.
    FTYPE V[NDIM];
    bl_coord_ijk(ii,jj,kk,loc,V) ;

    // V[X] is not same as Vmetric (used in set_gcov), so get it.
    FTYPE Vmetric[NDIM];
    rotate_VtoVmetric(MCOORD,V,Vmetric);
#endif

    FTYPE told, r, h, ph;
    told=Vmetric[0]; r=Vmetric[1]; h=Vmetric[2]; ph=Vmetric[3];
    // now have Vmetric and can put into trans below

    /* make transform matrix */
    // order for trans is [ourmetric][bl]
    // DLOOP(j,k) trans[j][k] = 0. ;
    // DLOOPA(j) trans[j][j] = 1. ;

    FTYPE b0=THETAROT;


    FTYPE trans[NDIM][NDIM];
    trans[0][0]=1.;
    trans[0][1]=0.;
    trans[0][2]=0.;
    trans[0][3]=0.;
    trans[1][0]=0.;
    trans[1][1]=1.;
    trans[1][2]=0.;
    trans[1][3]=0.;
    trans[2][0]=0.;
    trans[2][1]=0.;
    trans[2][2]=pow (pow (cos (h)*sin (b0) - 1.*cos (b0)*cos (ph)*sin (h),2.) + pow (sin (h),2.)*pow (sin (ph),2.),-0.5)*(-1.*cos (h)*cos (ph)*sin (b0) + cos (b0)*sin (h));
    trans[2][3]=pow (pow (cos (h)*sin (b0) - 1.*cos (b0)*cos (ph)*sin (h),2.) + pow (sin (h),2.)*pow (sin (ph),2.),-0.5)*sin (b0)*sin (h)*sin (ph);
    trans[3][0]=0.;
    trans[3][1]=0.;
    trans[3][2]=-1.*pow (pow (cos (h)*sin (b0) - 1.*cos (b0)*cos (ph)*sin (h),2.) + pow (sin (h),2.)*pow (sin (ph),2.),-1.)*sin (b0)*sin (ph);
    trans[3][3]=pow (pow (cos (h)*sin (b0) - 1.*cos (b0)*cos (ph)*sin (h),2.) + pow (sin (h),2.)*pow (sin (ph),2.),-1.)*sin (h)*(-1.*cos (h)*cos (ph)*sin (b0) + cos (b0)*sin (h));


    // now perform transformation
    transgcovgcovpertself(gcov,gcovpert,trans);
  }
  else{
    // then no change, and given function format gcov is just not changed.
  }

  /* done! */
}





// transformation of metric written in Vmetric-type coordinates with dVmetric differentials to one written in V[X] coordinates with dVdifferentials
//
// This is what's used to take original metric from set_gcov and get the one with new differentials based upon V[X]
void transVmetric2V(int whichcoord, int ii, int jj, int kk, int loc, FTYPE *X, FTYPE *V, FTYPE *Xmetric, FTYPE *Vmetric, FTYPE*gcov, FTYPE *gcovpert)
{

  if(THETAROT!=0.0 && ALLOWMETRICROT==1 && ISSPCMCOORD(whichcoord)){

#if(0)
    // gets V[X]
    FTYPE V[NDIM];
    bl_coord_ijk(ii,jj,kk,loc,V) ;

    // V[X] is not same as Vmetric (used in set_gcov)
    // trans needs Vmetric to keep expression simple, so get it.
    FTYPE Vmetric[NDIM];
    rotate_VtoVmetric(MCOORD,V,Vmetric);
    // now have Vmetric and can put into trans below
#endif
    FTYPE told, r, h, ph;
    told=Vmetric[0]; r=Vmetric[1]; h=Vmetric[2]; ph=Vmetric[3];

    /* make transform matrix */
    // order for trans is [ourmetric][bl]
    // DLOOP(j,k) trans[j][k] = 0. ;
    // DLOOPA(j) trans[j][j] = 1. ;

    FTYPE b0=THETAROT;

    FTYPE trans[NDIM][NDIM];
    extern FTYPE csc(FTYPE arg);
    extern FTYPE cot(FTYPE arg);

    trans[0][0]=1.;
    trans[0][1]=0.;
    trans[0][2]=0.;
    trans[0][3]=0.;
    trans[1][0]=0.;
    trans[1][1]=1.;
    trans[1][2]=0.;
    trans[1][3]=0.;
    trans[2][0]=0.;
    trans[2][1]=0.;
    trans[2][2]=pow (pow (cos (h)*cos (ph)*sin (b0) - 1.*cos (b0)*sin (h),2.) + pow (sin (b0),2.)*pow (sin (ph),2.),-1.)*pow (pow (cos (h)*sin (b0) - 1.*cos (b0)*cos (ph)*sin (h),2.) + pow (sin (h),2.)*pow (sin (ph),2.),0.5)*(-1.*cos (h)*cos (ph)*sin (b0) + cos (b0)*sin (h));
    trans[2][3]=-1.*sin (b0)*sin (ph);
    trans[3][0]=0.;
    trans[3][1]=0.;
    trans[3][2]=csc (h)*pow (pow (cos (h)*cos (ph)*sin (b0) - 1.*cos (b0)*sin (h),2.) + pow (sin (b0),2.)*pow (sin (ph),2.),-1.)*pow (pow (cos (h)*sin (b0) - 1.*cos (b0)*cos (ph)*sin (h),2.) + pow (sin (h),2.)*pow (sin (ph),2.),0.5)*sin (b0)*sin (ph);
    trans[3][3]=cos (b0) - 1.*cos (ph)*cot (h)*sin (b0);


  
    // now perform transformation
    transgcovgcovpertself(gcov,gcovpert,trans);
  }
  else{
    // then no change, and given function format gcov is just not changed.
  }


  /* done! */
}





// all below stuff independent of metrics

// convert primitive velocity to coordinate 4-velocity
int pr2ucon(int whichvel, FTYPE *pr, struct of_geom *geom, FTYPE*ucon)
{
  FTYPE others[NUMOTHERSTATERESULTS];

  if(whichvel==VEL4){
    // here pr has true 4-velocities, as supplied by init.c
    if (ucon_calc_4vel(pr, geom, ucon, others) >= 1) {
      dualfprintf(fail_file, "pr2ucon(ucon_calc): space-like error: whichvel=%d\n",whichvel);
      return(1);
    }
  }
  else if(whichvel==VEL3){ // supplied vel's are 3-vels
    if (ucon_calc_3vel(pr, geom, ucon, others) >= 1) {
      dualfprintf(fail_file, "pr2ucon(ucon_calc): space-like error: whichvel=%d\n",whichvel);
      return(1);
    }
  }
  else if(whichvel==VELREL4){ // supplied vel's are relative 4-vels
    if (ucon_calc_rel4vel(pr, geom, ucon, others) >= 1) {
      dualfprintf(fail_file, "pr2ucon(ucon_calc): space-like error: whichvel=%d\n",whichvel);
      return(1);
    }
  }
  else{
    dualfprintf(fail_file,"No such whichvel=%d\n",whichvel);
    myexit(1);
  }
  return(0);
}


void mettometp(int ii, int jj, int kk, FTYPE*ucon)
{
  int loc;

  loc=CENT;
  mettometp_genloc(ii, jj, kk, loc, ucon);

}

// MCOORD -> prime MCOORD
void mettometp_genloc(int ii, int jj, int kk, int loc, FTYPE*ucon)
{
  int j,k;
  FTYPE idxdxp[NDIM][NDIM];
  FTYPE tmp[NDIM];

  idxdxprim_ijk(ii, jj, kk, loc, idxdxp);

  // actually gcon_func() takes inverse of first arg and puts result into second arg.
  //  matrix_inverse(PRIMECOORDS, dxdxp,idxdxp);

  /* transform ucon */
  // this is u^j = (iT)^j_k u^k, as in mettobl() above
  DLOOPA(j) tmp[j] = 0.;
  DLOOP(j,k) tmp[j] += idxdxp[j][k] * ucon[k];
  DLOOPA(j) ucon[j] = tmp[j];
  
  // note that u_{k,BL} = u_{j,KSP} (iT)^j_k  

  // note that u_{k,KSP} = u_{j,BL} T^j_k  

  // note that u^{j,BL} =  T^j_k u^{k,KSP}   // (T) called ks2bl in grmhd-transforms.nb

  // note that u^{j,KSP} = (iT)^j_k u^{k,BL} // (iT) called bl2ks in grmhd-transforms.nb

  // So T=BL2KSP for covariant components and KSP2BL for contravariant components
  // and (iT)=BL2KSP for contra and KSP2BL for cov

  // where here T=dxdxp and (iT)=idxdxp (not transposed, just inverse)

  /* done! */
}

// MCOORD -> prime MCOORD for u^\mu
void mettometp_simple(FTYPE (*idxdxp)[NDIM], FTYPE*ucon)
{
  int j,k;
  FTYPE X[NDIM], V[NDIM];
  FTYPE tmp[NDIM];

  /* transform ucon */
  // this is u^j = (iT)^j_k u^k, as in mettobl() above
  DLOOPA(j) tmp[j] = 0.;
  DLOOP(j,k) tmp[j] += idxdxp[j][k] * ucon[k];
  DLOOPA(j) ucon[j] = tmp[j];
}

// prime MCOORD -> MCOORD for u_\mu
void metptomet_ucov_simple(FTYPE (*idxdxp)[NDIM], FTYPE*ucov)
{
  int j,k;
  FTYPE X[NDIM], V[NDIM];
  FTYPE tmp[NDIM];

  /* transform ucov */
  DLOOPA(j) tmp[j] = 0.;
  DLOOP(j,k) tmp[j] += idxdxp[k][j] * ucov[k];
  DLOOPA(j) ucov[j] = tmp[j];
}


void metptomet(int ii, int jj, int kk, FTYPE*ucon)
{
  int loc;

  loc=CENT;
  metptomet_genloc(ii, jj, kk, loc, ucon);

}

// prime MCOORD -> MCOORD for u^\mu
void metptomet_genloc(int ii, int jj, int kk, int loc, FTYPE*ucon)
{
  int j,k;
  FTYPE dxdxp[NDIM][NDIM];
  FTYPE tmp[NDIM];

  dxdxprim_ijk(ii, jj, kk, loc, dxdxp);

  /* transform ucon */
  // this is u^j = T^j_k u^k, as in above
  DLOOPA(j) tmp[j] = 0.;
  DLOOP(j,k) tmp[j] += dxdxp[j][k] * ucon[k];
  DLOOPA(j) ucon[j] = tmp[j];

  /* done! */
}

// prime MCOORD -> MCOORD for u^\mu
void metptomet_simple(FTYPE (*dxdxp)[NDIM], FTYPE*ucon)
{
  int j,k;
  FTYPE tmp[NDIM];

  /* transform ucon */
  // this is u^j = T^j_k u^k, as in above
  DLOOPA(j) tmp[j] = 0.;
  DLOOP(j,k) tmp[j] += dxdxp[j][k] * ucon[k];
  DLOOPA(j) ucon[j] = tmp[j];

  /* done! */
}

// MCOORD -> prime MCOORD for u_\mu
void mettometp_ucov_simple(FTYPE (*dxdxp)[NDIM], FTYPE*ucov)
{
  int j,k;
  FTYPE tmp[NDIM];

  /* transform ucov */
  DLOOPA(j) tmp[j] = 0.;
  DLOOP(j,k) tmp[j] += dxdxp[k][j] * ucov[k];
  DLOOPA(j) ucov[j] = tmp[j];

  /* done! */
}

// prime MCOORD -> MCOORD for T^\mu_\nu
void metptomet_Tud(int ii, int jj, int kk, FTYPE (*Tud)[NDIM])
{
  int j,k;
  int alpha,beta;
  FTYPE V[NDIM], X[NDIM];
  FTYPE dxdxp[NDIM][NDIM];
  FTYPE idxdxp[NDIM][NDIM];
  FTYPE tmp[NDIM][NDIM];
  void metptomet_simple_Tud(FTYPE (*dxdxp)[NDIM], FTYPE (*idxdxp)[NDIM], FTYPE (*Tud)[NDIM]);
  int loc;


  loc=CENT;
  dxdxprim_ijk(ii, jj, kk, loc, dxdxp);
  idxdxprim_ijk(ii, jj, kk, loc, dxdxp);

  metptomet_simple_Tud(dxdxp, idxdxp, Tud);

  /* done! */
}

// prime MCOORD -> MCOORD
// feed in coordinate dependent quantities instead of computing them
// useful when wanting to speep up calculation when calling function needs dxdxp and/or idxdxp
void metptomet_simple_Tud(FTYPE (*dxdxp)[NDIM], FTYPE (*idxdxp)[NDIM], FTYPE (*Tud)[NDIM])
{
  int j,k;
  int alpha,beta;
  FTYPE tmp[NDIM][NDIM];

  // Tud^j_k [KS] = T^\beta_\alpha [KS'] ((Lambda)^{-1})^\alpha_k \Lambda^j_\beta
  DLOOP(j,k) tmp[j][k] = 0.;
  DLOOP(alpha,beta) DLOOP(j,k) tmp[j][k] += idxdxp[alpha][k] * dxdxp[j][beta] * Tud[beta][alpha];
  DLOOP(j,k) Tud[j][k] = tmp[j][k];

  /* done! */
}

// convert 4-velocity to whichvel velocity
void ucon2pr(int whichvel, FTYPE *ucon, struct of_geom *geom, FTYPE *pr)
{
  FTYPE alphasq,gammaoalpha,beta[NDIM] ;
  int j;

  if(whichvel==VEL4){
    SLOOPA(j) pr[U1+j-1]=ucon[j];
  }
  if(whichvel==VEL3){
    SLOOPA(j) pr[U1+j-1] = ucon[j] / ucon[TT];
  }
  else if(whichvel==VELREL4){
    alphasq = 1./(-geom->gcon[GIND(TT,TT)]) ;
    gammaoalpha = ucon[TT] ;

    SLOOPA(j) beta[j] = alphasq*geom->gcon[GIND(TT,j)] ;
    SLOOPA(j) pr[U1+j-1] = ucon[j] + beta[j]*gammaoalpha ;
  }
}

// convert 3-velocity to whichvel velocity
int vcon2pr(int whichvel, FTYPE *vcon, struct of_geom *geom, FTYPE *pr)
{
  FTYPE alphasq,gammaoalpha,beta[NDIM] ;
  int j;
  FTYPE ucon[NDIM];
  FTYPE others[NUMOTHERSTATERESULTS];
  FTYPE prlocal[NPR];

  SLOOPA(j) prlocal[U1+j-1]=vcon[j];

  if(whichvel==VEL4){
    // vcon->ucon

    if(ucon_calc_3vel(prlocal, geom, ucon, others)>=1){
      dualfprintf(fail_file, "vcon2pr(ucon_calc_3vel): space-like error\n");
      return(1);
    }

    // ucon->pr
    //    SLOOPA(j) pr[U1+j-1]=vcon[j]*ucon[TT];
    // or just directly pr=ucon
    SLOOPA(j) pr[U1+j-1]=ucon[j];
  }
  if(whichvel==VEL3){
    // vcon=pr
    SLOOPA(j) pr[U1+j-1] = vcon[j] ;
  }
  else if(whichvel==VELREL4){
    // vcon->ucon
    if(ucon_calc_3vel(prlocal, geom, ucon, others)>=1){
      dualfprintf(fail_file, "vcon2pr(ucon_calc_3vel): space-like error\n");
      return(1);
    }

    // now go from ucon->pr
    alphasq = 1./(-geom->gcon[GIND(TT,TT)]) ;
    gammaoalpha = ucon[TT] ;

    SLOOPA(j) beta[j] = alphasq*geom->gcon[GIND(TT,j)] ;
    SLOOPA(j) pr[U1+j-1] = ucon[j] + beta[j]*gammaoalpha ;
  }
  return(0);
}

////////////////////////
//
// below not used

#define NORMALDENSITY 0
#define LOGDENSITY 1

#define DENSITYTYPE LOGDENSITY


// make sure both of these are setup so density could be same memory location as pr
void density2pr(FTYPE *density, FTYPE *pr)
{

#if(DENSITYTYPE==NORMALDENSITY)
  density[RHO]=pr[RHO];
  density[UU]=pr[UU];
#elif(DENSITYTYPE==LOGDENSITY)
  density[RHO]=log(pr[RHO]);
  density[UU]=log(pr[UU]);
#endif

}

// note that we have to have inverses for this to work in general, numerical inverses probably bad idea?
void pr2density(FTYPE *pr, FTYPE *density)
{

#if(DENSITYTYPE==NORMALDENSITY)
  pr[RHO]=density[RHO];
  pr[UU]=density[UU];
#elif(DENSITYTYPE==LOGDENSITY)
  pr[RHO]=exp(density[RHO]);
  pr[UU]=exp(density[UU]);
#endif

}
