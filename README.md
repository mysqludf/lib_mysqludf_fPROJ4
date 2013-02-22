LIB\_MYSQLUDF\_FPROJ4
===================

This library is based on program PROJ.4 which is a standard Unix filter function which converts geographic longitude and latitude coordinates into cartesian coordinates, by means of a wide variety of cartographic projection functions. It is in active use by GRASS GIS, MapServer, PostGIS, Thuban, OGDI, Mapnik, TopoCad, and OGRCoordinateTransformation as well as various other projects. Full description and documentation of PROJ.4 can be found at official website of the project. 

Some selected API functions of PROJ.4 are ported to MySQL within the library name of lib\_mysqludf\_fPROJ4 which is a sub package of the project MySci. The code is transfered to the coding standard of mysqludf.org and this distribution has LGPL license as any other mysqludf.org libraries.

INSTALL
-------

The PROJ.4 library is NOT build into this code. You have to optain PROJ.4 package by yourself. You can either get the source code and compile on your system or place the compiled dynamic API library to a location which is in your PATH. You can find both at http://trac.osgeo.org/proj/
Thereafter;

* If you are using the Windows binary, you just place the DLL file under your MySQL Plugin directory and load the functions with lib\_mysqludf\_fPROJ4.sql script.
* If you are using the source code, you need to include the PROJ.4 include directory and add the proj\_i static library to your build.

FUNCTIONS
---------

###lib\_mysqludf\_fPROJ4\_info()###

Function takes no argument and return the library version of the itself and the PROJ4 library version used in the system as single string.
Usage:

```
SELECT lib_mysqludf_fPROJ4_info();
+---------------------------------------------------------------------------------+
| lib_mysqludf_fPROJ4_info()                                                      |
+---------------------------------------------------------------------------------+
| lib_mysqludf_udf version 0.1 (2010.05.09) (PROJ4 Rel. 4.7.1, 23 September 2009) |
+---------------------------------------------------------------------------------+
```

###fPROJ4\_Geo2UTM (REAL x, REAL y, INT zone, INT compenant\_to\_return)###

Function takes 4 arguments and transfers the input from Geographic Coordinate System to UTM. 

* First 2 arguments are the position of the point in space as {x,y}. Input MUST be DEGREES.
* Third arguments is the zone you will transfer to.
* Last argument defines the returned value. Which can be
   - 0 for x
   - 1 for y

Usage: In this example the highest point coordinates of Mount Olympus in Greece, which is given with Geographic Coordinate System is projected to UTM with datum WGS84.

```
SELECT
  fPROJ4_Geo2UTM(22.350,40.084,34,0) as X,
  fPROJ4_Geo2UTM(22.350,40.084,34,1) as Y;
+---------------+----------------+
| X             | Y              |
+---------------+----------------+
| 615096.109638 | 4437953.659204 |
+---------------+----------------+
```

###fPROJ4\_UTM2Geo (REAL x, REAL y, INT zone, INT compenant\_to\_return)###

Function takes 4 arguments and transfers the input from UTM to Geographic Coordinate System. 

* First 2 arguments are the position of the point in space as {x,y}. Input MUST be METRES.
* Third arguments is the zone you will transfer from.
* Last argument defines the returned value. Which can be
   - 0 for x
   - 1 for y

Usage: In this example the highest point coordinates of Mount Olympus in Greece, which is given with UTM is projected to Geographic Coordinate System with datum WGS84.

```
SELECT
  fPROJ4_UTM2Geo(615096.109638,4437953.659204,34,0) as X,
  fPROJ4_UTM2Geo(615096.109638,4437953.659204,34,1) as Y;
+---------------+----------------+
| X             | Y              |
+---------------+----------------+
| 22.3500000000 | 40.0840000000  |
+---------------+----------------+
```

###fPROJ4\_transform (REAL x, REAL y, REAL z, STRING projection\_from, STRING projection\_to, INT compenant\_to\_return)###

*This is an advanced function for projecting anything to anything. You can change the elipsoid, datum etc. USE IT WITH CARE AND READ THE PROJ4 DOCUMENTATION.*

Function takes 6 arguments and transfer the coordinates from one projection to another. 

* First 2 arguments are the position of the point in space as {x,y}. Your input MUST be related to your definition of projection\_from. For example, for geographic coordinate systems input MUST be RADIANS and UTM coordinate systems input MUST be in metres.
* Third argument is the height in metres. Most of the map projections z values will return the same input value; but I kept in the library for arbitary projections.
* Next 2 arguments are the projection definitions of FROM and TO. You need to consult to the original PROJ.4 documentation for this input.
* Last argument defines the returned value. Which can be
   - 0 for x
   - 1 for y
   - 2 for z

Usage: In this example the highest point coordinates of Mount Olympus in Greece, which is given with Geographic Coordinate System is projected to UTM with datum WGS84.

```
SELECT
fPROJ4_transform(RADIANS(22.350),RADIANS(40.084),2843,
 '+proj=latlong +ellps=clrk66','+proj=utm +datum=WGS84 +zone=34',0) as X,
fPROJ4_transform(RADIANS(22.350),RADIANS(40.084),2843,
 '+proj=latlong +ellps=clrk66','+proj=utm +datum=WGS84 +zone=34',1) as Y,
fPROJ4_transform(RADIANS(22.350),RADIANS(40.084),2843,
 '+proj=latlong +ellps=clrk66','+proj=utm +datum=WGS84 +zone=34',2) as Z;
+---------------+----------------+-------------+
| X             | Y              | Z           |
+---------------+----------------+-------------+
| 615096.109638 | 4437953.659204 | 2843.000000 |
+---------------+----------------+-------------+
```
