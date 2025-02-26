#ifndef        HMODEL_H
#define HMODEL_H

#include "common.h"

uniform samplerCUBE         env_s0                ;
uniform samplerCUBE         env_s1                ;

uniform half4                         env_color        ;        // color.w  = lerp factor
uniform half3x4                        m_v2w                ;

void        hmodel                 (out half3 hdiffuse, out half3 hspecular, half m, half h, half s, float3 point, half3 normal)
{
	///////////////////////////////////////////////////////////////////////////////////////
        // !hscale - Some odd form of diffuse lighting created using the CUBEmap
	///////////////////////////////////////////////////////////////////////////////////////
	 half3         nw                         = mul                 (m_v2w,normal);
        half         hscale                 = h;                                //. *        (.5h + .5h*nw.y);

                        //hscale                = (hscale*hscale);        // make it more linear

	///////////////////////////////////////////////////////////////////////////////////////
        // !Phong reflection vector
	// D = Reflection f = 
	///////////////////////////////////////////////////////////////////////////////////////

	half3 N		= normal;						 
  	half3 V 	= normalize		(point);
	half3 Vx        = mul			(m_v2w,V);			
  	half3 L 	= -Ldynamic_dir;					
	half3 R     	= reflect         	(Vx,N);

	half 	d	= saturate(dot(R,Vx));
	half  f		= saturate(dot(R,-L));
			     d *= f;
		    		
			


  	
	
		  
	//////////////////////////////////////////////////////////////////////////////////////////////
        // !Surface material
          half4         light                = tex3D                (s_material, half3(hscale, d, m) );
		                
			

        //////////////////////////////////////////////////////////////////////////////////////
        // diffuse color - !Combined 4 texture lookups into only two for both diff and spec
	//////////////////////////////////////////////////////////////////////////////////////
        half3         e0d               = texCUBE         (env_s0,nw);
        half3         e1d               = texCUBE         (env_s1,nw);
        half3         env_d             = env_color.xyz*lerp(e0d,e1d,env_color.w)        ;
					env_d*=env_d;		// contrast

        hdiffuse                        = (env_d) * (light.xyz) + L_ambient.rgb;
                                                                     
        hspecular                       = env_d * (light.w)*s;                
}
	//////////////////////////////////////////////////////////////////////////////////////
	//  !The code below is for Object dynamic and isn't used for our purposes
	//   but I have left it for compatibility. It is not compiled at run-time.
	//////////////////////////////////////////////////////////////////////////////////////


void         hmodel_table        (out half3 hdiffuse, out half3 hspecular, half m, half h, half s, half3 point, half3 normal)
{
        // hscale - something like diffuse reflection
        half         hscale         = h;

        // reflection vector
        half3         v2point        = normalize        (point);
        half3        vreflect= reflect         (v2point,normal);
        half         hspec         = .5h+.5h*dot        (vreflect,v2point);

        // material
          half4         light        = tex3D                (s_material, half3(hscale, hspec, m) );                // sample material

        // diffuse color
        half3         env_d         = texCUBE         (env_s0,normal);

        // specular color
        half3         env_s          = texCUBE         (env_s0,vreflect);

        //
        hdiffuse        = env_d        *light.xyz         + L_ambient.rgb        ;
        hspecular        = env_s        *light.w         * s                ;
}

#endif
