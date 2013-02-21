/*

/* 
	lib_mysqludf_fPROJ4 - An extended set of scientific functions which 
	converts geographic longitude and latitude coordinates into cartesian 
	coordinates and vice versa.
	
	Copyright (C) 2010  Ferhat Bingöl
	web: http://www.mysqludf.org/ 
	e-mail: ferhat.bingol@gmail.com   
	
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
	
	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.
	
	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(WIN32)
#define DLLEXP __declspec(dllexport) 
#else
#define DLLEXP
#endif

#ifdef STANDARD
#include <string.h>
#include <stdlib.h>
#include <time.h>
#ifdef __WIN__
typedef unsigned __int64 ulonglong;
typedef __int64 longlong;
#else
typedef unsigned long long ulonglong;
typedef long long longlong;
#endif /*__WIN__*/
#else

#include <my_global.h>
#include <my_sys.h>
#endif
#include <proj_api.h>
#include <mysql.h>

#include <m_ctype.h>
#include <m_string.h>
#include <stdlib.h>
#include <string.h>


#include <ctype.h>



#ifdef HAVE_DLOPEN

#define LIBVERSION "lib_mysqludf_fPROJ4 version 0.1 (2010.05.09)"


#ifdef	__cplusplus
extern "C" {
#endif



// fPROJ4_info
// LIBRARY INFORMATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DLLEXP my_bool lib_mysqludf_fPROJ4_info_init(UDF_INIT *initid,UDF_ARGS *args, char *message);
DLLEXP void	lib_mysqludf_fPROJ4_info_deinit(UDF_INIT *initid);
DLLEXP char* lib_mysqludf_fPROJ4_info(UDF_INIT *initid,UDF_ARGS *args,char* result, unsigned long* length, char *is_null, char *error);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// FUNCTION: 
//	fPROJ4_transform 
// PORTED FROM: 
//	int pj_transform( projPJ src, projPJ dst, long point_count, int point_offset, double *x, double *y, double *z );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DLLEXP my_bool fPROJ4_transform_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
DLLEXP void fPROJ4_transform_deinit(UDF_INIT *initid);
DLLEXP double fPROJ4_transform(UDF_INIT *initid,UDF_ARGS *args,char *is_null,char *error);

// FUNCTION: 
//	fPROJ4_Geo2UTM 
// MySci specific function, based on pj_transform(...)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DLLEXP my_bool fPROJ4_Geo2UTM_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
DLLEXP void fPROJ4_Geo2UTM_deinit(UDF_INIT *initid);
DLLEXP double fPROJ4_Geo2UTM(UDF_INIT *initid,UDF_ARGS *args,char *is_null,char *error);

// FUNCTION: 
//	fPROJ4_UTM2Geo 
// MySci specific function, based on pj_transform(...)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DLLEXP my_bool fPROJ4_UTM2Geo_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
DLLEXP void fPROJ4_UTM2Geo_deinit(UDF_INIT *initid);
DLLEXP double fPROJ4_UTM2Geo(UDF_INIT *initid,UDF_ARGS *args,char *is_null,char *error);


#ifdef	__cplusplus
}
#endif


my_bool lib_mysqludf_fPROJ4_info_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	my_bool status;
	if(args->arg_count!=0)
	{
		strcpy(message,"No arguments allowed (usage: lib_mysqludf_fPROJ4_info() )");
		status = 1;
	} 
	else status = 0;

	return status;
}
void lib_mysqludf_fPROJ4_info_deinit(UDF_INIT *initid)
{
	// Nothing to do
}
char* lib_mysqludf_fPROJ4_info(UDF_INIT *initid,UDF_ARGS *args,char* result,unsigned long* length,char *is_null,char *error)
{
	char temp[100];
	const char *release;
	release=pj_get_release();
	sprintf(temp,"%s (PROJ4 %s)",LIBVERSION,release);
	strcpy(result,temp);
	*length = strlen(result);
	return result;
}

// FUNCTION: fPROJ4_transform
//////////////////////////////////////////////////////////////////////////////////////////////////////////
my_bool fPROJ4_transform_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	my_bool status;
	if(args->arg_count!=6)
	{
		strcpy(	message	,"Input 6 argument(s) (usage: fPROJ4_transform(double x, double y, double z, char *projPJ_src, char *projPJ_dst, int xyz) )");
		status = 1;
	} 
	else if(!(initid->ptr = malloc(sizeof(double))))
	{
		initid->ptr = NULL;
		strcpy(message,	"Could not allocate memory (fPROJ4_transform)");
		status = 1;
	} 
	else 
	{
		initid->maybe_null= 1;
		initid->decimals= 6;
				
		args->arg_type[0] = REAL_RESULT;
		args->arg_type[1] = REAL_RESULT;
		args->arg_type[2] = REAL_RESULT;
		args->arg_type[3] = STRING_RESULT;
		args->arg_type[4] = STRING_RESULT;
		args->arg_type[5] = INT_RESULT;
						
		*((double *)initid->ptr) = 0;
		status = 0;

	}

	return status;
}
void fPROJ4_transform_deinit(UDF_INIT *initid)
{
	if(initid->ptr!=NULL)	free(initid->ptr);
}
double fPROJ4_transform(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	projPJ pj_in, pj_out;
	double x, y,z;
	int p,t;

	if(args->args[5]!=NULL && atoi(args->args[5])<3)
	{
		x=*((const double*)args->args[0]);
		y=*((const double*)args->args[1]);
		z=*((const double*)args->args[2]);
		//x *= DEG_TO_RAD;
		//y *= DEG_TO_RAD;
		if (!(pj_in = pj_init_plus(args->args[3]) ) )  exit(0);
		if (!(pj_out = pj_init_plus(args->args[4]) ) )  exit(0);
		t=*((const int*)args->args[5]);

		p = pj_transform(pj_in, pj_out, 1, 1, &x, &y, &z );
						
		if(t==0) *((double *)initid->ptr) = x; 
		else if(t==1) *((double *)initid->ptr) = y; 
		else if(t==2) *((double *)initid->ptr) = z; 
		else *((double *)initid->ptr) = 0.0; 
	} 
	else *((double *)initid->ptr) = 0.0; 
	
	pj_free(pj_in);
	pj_free(pj_out);
	
	return *((double *)initid->ptr);
}


// FUNCTION: fPROJ4_Geo2UTM
//////////////////////////////////////////////////////////////////////////////////////////////////////////
my_bool fPROJ4_Geo2UTM_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	my_bool status;
	if(args->arg_count!=4)
	{
		strcpy(	message	,"Input 4 argument(s) (usage: fPROJ4_Geo2UTM(double x, double y, int zone, int xyz) )");
		status = 1;
	} 
	else if(!(initid->ptr = malloc(sizeof(double))))
	{
		initid->ptr = NULL;
		strcpy(message,	"Could not allocate memory (fPROJ4_Geo2UTM)");
		status = 1;
	} 
	else 
	{
		initid->maybe_null= 1;
		initid->decimals= 10;
				
		args->arg_type[0] = REAL_RESULT;
		args->arg_type[1] = REAL_RESULT;
		args->arg_type[2] = INT_RESULT;
		args->arg_type[3] = INT_RESULT;
						
		*((double *)initid->ptr) = 0;
		status = 0;

	}

	return status;
}
void fPROJ4_Geo2UTM_deinit(UDF_INIT *initid)
{
	if(initid->ptr!=NULL)	free(initid->ptr);
}
double fPROJ4_Geo2UTM(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	projPJ pj_in, pj_out;
	double x, y;
	int p,t,zone;
	char *tmp;

	tmp=(char*)malloc(sizeof(char)*200);

	if(args->args[3]!=NULL && atoi(args->args[3])<2)
	{
		x=*((const double*)args->args[0]);
		y=*((const double*)args->args[1]);
		x *= DEG_TO_RAD;
		y *= DEG_TO_RAD;
		zone=*((const int*)args->args[2]);
		if (!(pj_in = pj_init_plus("+proj=latlong +ellps=clrk66") ) )  exit(0);
		sprintf(tmp,"+proj=utm +datum=WGS84 +zone=%i",zone);
		if (!(pj_out = pj_init_plus(tmp) ) )  exit(0);
		free(tmp);
		t=*((const int*)args->args[3]);

		p = pj_transform(pj_in, pj_out, 1, 1, &x, &y, 0 );
						
		if(t==0) *((double *)initid->ptr) = x; 
		else if(t==1) *((double *)initid->ptr) = y; 
		else *((double *)initid->ptr) = 0.0; 
	} 
	else *((double *)initid->ptr) = 0.0; 
	
	pj_free(pj_in);
	pj_free(pj_out);
	
	return *((double *)initid->ptr);
}



// FUNCTION: fPROJ4_UTM2Geo
//////////////////////////////////////////////////////////////////////////////////////////////////////////
my_bool fPROJ4_UTM2Geo_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	my_bool status;
	if(args->arg_count!=4)
	{
		strcpy(	message	,"Input 4 argument(s) (usage: fPROJ4_UTM2Geo(double x, double y, int zone, int xyz) )");
		status = 1;
	} 
	else if(!(initid->ptr = malloc(sizeof(double))))
	{
		initid->ptr = NULL;
		strcpy(message,	"Could not allocate memory (fPROJ4_UTM2Geo)");
		status = 1;
	} 
	else 
	{
		initid->maybe_null= 1;
		initid->decimals= 10;
				
		args->arg_type[0] = REAL_RESULT;
		args->arg_type[1] = REAL_RESULT;
		args->arg_type[2] = INT_RESULT;
		args->arg_type[3] = INT_RESULT;
						
		*((double *)initid->ptr) = 0;
		status = 0;

	}

	return status;
}
void fPROJ4_UTM2Geo_deinit(UDF_INIT *initid)
{
	if(initid->ptr!=NULL)	free(initid->ptr);
}
double fPROJ4_UTM2Geo(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	projPJ pj_in, pj_out;
	double x, y;
	int p,t,zone;
	char *tmp;

	tmp=(char*)malloc(sizeof(char)*200);

	if(args->args[3]!=NULL && atoi(args->args[3])<2)
	{
		x=*((const double*)args->args[0]);
		y=*((const double*)args->args[1]);
		//x *= DEG_TO_RAD;
		//y *= DEG_TO_RAD;
		zone=*((const int*)args->args[2]);
		sprintf(tmp,"+proj=utm +datum=WGS84 +zone=%i",zone);
		if (!(pj_in = pj_init_plus(tmp) ) )  exit(0);
		
		if (!(pj_out = pj_init_plus("+proj=latlong +ellps=clrk66") ) )  exit(0);
		free(tmp);
		t=*((const int*)args->args[3]);

		p = pj_transform(pj_in, pj_out, 1, 1, &x, &y, 0 );
						
		if(t==0) *((double *)initid->ptr) = x/DEG_TO_RAD; 
		else if(t==1) *((double *)initid->ptr) = y/DEG_TO_RAD; 
		else *((double *)initid->ptr) = 0.0; 
	} 
	else *((double *)initid->ptr) = 0.0; 
	
	pj_free(pj_in);
	pj_free(pj_out);
	
	return *((double *)initid->ptr);
}
#endif /* HAVE_DLOPEN */

