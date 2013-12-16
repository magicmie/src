/* Local prediction filter for complex numbers (n-dimensional). */
/*
  Copyright (C) 2006 University of Texas at Austin
   
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
   
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
   
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <rsf.h>

#include "cmultidivn.h"
#include "cweight2.h"

int main(int argc, char* argv[])
{
    bool verb;
    int m[1], rect[1];
    int i1,iw,niter,nshift,n1,n2,n12,ns,i3,n3,i,ii1,is,jump;
    sf_complex *d1,**d,*sh, *f, *one, *d1j, *shj;
    float mean,*mk;
    sf_file dat, flt, mask, pre, zshift, zdata;

    sf_init(argc,argv);

    dat = sf_input("in");
    flt = sf_output("out");
    /*
    if (NULL != sf_getstring("mask")) {
        mask = sf_input("mask");
    }
    */
    if (!sf_getint("rect",rect)) rect[0]=10;
	/* smoothing radius (in space X) */

    if (!sf_getint("ns",&ns)) ns=1;
	/* shifts of both sides npef=2*ns+1*/

    if (!sf_getint("niter",&niter)) niter=100;
    /* number of iterations */

    if (!sf_getbool("verb",&verb)) verb=true;
    /* verbosity flag */

    if (SF_COMPLEX != sf_gettype(dat)) sf_error("Need complex input");

    if (!sf_getint("jump",&jump)) jump=1;
    /* jump */

    if (!sf_histint(dat,"n1",&n1)) sf_error("No n1= in input");
    if (!sf_histint(dat,"n2",&n2)) sf_error("No n2= in input");



/*
    if (NULL != sf_getstring("pred")) {
    	pre = sf_output("pred");
    } else {
    	pre = NULL;
    }


    if (NULL != sf_getstring("zshift")) {
	    zshift = sf_output("zshift");
        sf_shiftdim(dat, zshift, 2);
        sf_putint(zshift,"n2",2*ns);
        sf_putfloat(zshift,"d2",1);
        sf_putfloat(zshift,"o2",-ns);
    } else {
	    zshift = NULL;
    }


    if (NULL != sf_getstring("zdata")) {
	    zdata = sf_output("zdata");
    } else {
	    zdata = NULL;
    }
    
    */
    n12=n1*n2;
    n3 = sf_leftsize(dat,2);



    nshift=n1*2*ns;
    
    sf_shiftdim(dat, flt, 2);
    sf_putint(flt,"n1",n1*jump);
    sf_putint(flt,"n2",2*ns+1);
    sf_putfloat(flt,"d2",1);
    sf_putfloat(flt,"o2",-ns);

    d  = sf_complexalloc2(n1,n2);
    d1 = sf_complexalloc(n1);
    d1j = sf_complexalloc(n1*jump);
    one = sf_complexalloc(n1*jump);
    sh = sf_complexalloc(nshift);
    shj = sf_complexalloc(nshift*jump);
    //mk = sf_floatalloc(n1);
    f  = sf_complexalloc(nshift*jump);
    //f = sf_complexalloc(n12);
    //g = sf_complexalloc(nd);
    m[0]=n1*jump;
    
    for (i1=0;i1<n1*jump;i1++) one[i1]=sf_cmplx(-1,0);

    for (i3=0; i3<n3; i3++) { // y

        sf_complexread(d[0],n12,dat);
/*
        if (NULL != sf_getstring("mask")) {
            sf_floatread(mk,n1,mask);
        } else {
            for (i1=0;i1<n1;i1++) mk[i1]=1.0;
        }
*/
         for (iw=0; iw<n2; iw++) { // w
	          sf_warning("iw = %d of %d", iw, n2);

              for (i1=0;i1<n1;i1++){
                   d1[i1] = d[iw][i1];
                   for (is=0; is < (2*ns); is++) {
                        sh[is*n1+i1]=sf_cmplx(0,0);
                   }
              }
              // shifts
              for (is=0;is<ns;is++) {//forward
                   for (i1=0;i1<is+1;i1++) {
                        sh[n1*ns+is*n1+i1]=sf_cmplx(0,0);
                   }
                   for (i1=is+1;i1<n1;i1++) {
                        sh[n1*ns+is*n1+i1]=d1[i1-is-1];
                   }
                   
              //}
             // for (is=0;is<ns;is++) { // backward
                   for (i1=0;i1<n1-ns+is-1;i1++) {
                        sh[is*n1+i1]=d1[i1+ns-is];
                   }
                   for (i1=n1-ns+is-1;i1<n1;i1++) {
                        sh[is*n1+i1]=sf_cmplx(0,0);
                   }
                   
              }                  
              // shift end
              //Shifts end
              for (i1=0;i1<n1*jump;i1++){
                    d1j[i1]=sf_cmplx(0,0);
              }
              for (i1=0;i1<nshift*jump;i1++){
                    shj[i1]=sf_cmplx(0,0);
              }
              for (i1=0;i1<n1;i1++){
                    d1j[i1*jump]=d1[i1];
                    for (is=0; is < (2*ns); is++) {
                        shj[is*n1*jump+i1*jump]=sh[is*n1+i1];
                   }
              }

              
              
/*
              for (i1=0;i1<n1;i1++) { //Zero mask
                   if (mk[i1] == 0.) { 
                       for (ii1=i1-ns;ii1<i1+ns+1;ii1++){
                            if(ii1 >= 0 && ii1< n1){
                                 d1[ii1] = sf_cmplx(0,0);
                                 for (is=0; is < (2*ns); is++) {
                                      sh[is*n1+ii1]=sf_cmplx(0,0);
                                 }
                            }
                       }
                   } 
                   
              }         
              if (zshift) {
                  sf_complexwrite(sh,nshift,zshift);
              }
              if (zdata) {
                  sf_complexwrite(d1,n1,zdata);
              }
*/              
             cmultidivn_init(2*ns, 1, n1*jump, m, rect, shj, false);    

             mean = 0.;
             for(i = 0; i < nshift*jump; i++) {
#ifdef SF_HAS_COMPLEX_H
	         mean += crealf(conjf(shj[i])*shj[i]);
#else
	         mean += crealf(sf_cmul(conjf(shj[i]),shj[i]));
#endif
             }
             if (mean == 0.) {
                 for(i=0; i < nshift*jump; i++) {
                     f[i]=sf_cmplx(0,0); 
                 }
                 sf_complexwrite(f,n1*jump*ns,flt);  
	             sf_complexwrite(one,n1*jump,flt);  
	             sf_complexwrite(f+ns*n1*jump,n1*jump*ns,flt);  
	             continue;
             }
             sf_warning("iw=%d and mean=%g",iw, mean);
             mean = sqrtf (nshift/mean);

    
            for(i=0; i < nshift*jump; i++) {
#ifdef SF_HAS_COMPLEX_H
	            shj[i] *= mean;
#else
 	            shj[i] = sf_crmul(shj[i],mean);
#endif
            }
            for(i=0; i < n1*jump; i++) {
#ifdef SF_HAS_COMPLEX_H
	            d1j[i] *= mean;
#else
	            d1j[i] = sf_crmul(d1j[i],mean);
#endif
            }

            cmultidivn (d1j,f,niter);  
            sf_complexwrite(f,n1*jump*ns,flt);  
	        sf_complexwrite(one,n1*jump,flt);  
	        sf_complexwrite(f+ns*n1*jump,n1*jump*ns,flt);  
/*
            if (pre) {
	            for(i=0; i < nshift; i++) {
#ifdef SF_HAS_COMPLEX_H
	                sh[i] /= mean;
#else
	                sh[i] = sf_crmul(sh[i],1./mean);
#endif
	            }
	
	            cweight2_lop(false,false,nshift,n1,f,d1);
	            sf_complexwrite(d1,n1,pre);
            }
*/

         } // w end
    } // y end



    exit(0);
}