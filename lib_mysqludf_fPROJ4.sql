DROP FUNCTION IF EXISTS lib_mysqludf_fPROJ4_info;
create function lib_mysqludf_fPROJ4_info returns string soname 'lib_mysqludf_fPROJ4.dll';

DROP FUNCTION IF EXISTS fPROJ4_transform;
create function fPROJ4_transform returns real soname 'lib_mysqludf_fPROJ4';

DROP FUNCTION IF EXISTS fPROJ4_Geo2UTM;
create function fPROJ4_Geo2UTM returns real soname 'lib_mysqludf_fPROJ4';

DROP FUNCTION IF EXISTS fPROJ4_UTM2Geo;
create function fPROJ4_UTM2Geo returns real soname 'lib_mysqludf_fPROJ4';