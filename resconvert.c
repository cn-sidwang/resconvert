#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int _convert( const char *filename )
{
	int    i = 0;
	FILE   *src_fp = NULL;
	FILE   *dst_fp = NULL;
	char   basename[256] = {0};
	char   dstname[256] = {0};
	char   marconame_upper[256] = {0};
	char   marconame_lower[256] = {0};
	char   *base_start = NULL;
	size_t src_file_size = 0;
	size_t read_count = 0;
	size_t read_size = 0;
	unsigned char buffer[2048] = {0};

	if( (base_start = strrchr( filename, '/' )) != NULL )
	{
		strcpy( basename, base_start+1 );
	}
	else
	{
		strcpy( basename, filename );
	}
	
	for( i = 0; i<strlen(basename); i++ )
	{
		dstname[i] = basename[i]=='.'? '_' : basename[i];
	}
	strcat( dstname, ".h" );
	for( i=0; i<strlen(dstname); i++ )
	{
		marconame_upper[i] = toupper( dstname[i]=='.'? '_' : dstname[i] );
		marconame_lower[i] = tolower( dstname[i]=='.'? '_' : dstname[i] );
	}
	
	src_fp = fopen( filename, "rb" );
	if( src_fp == NULL )
	{
		return -1;
	}

	dst_fp = fopen( dstname, "w" );
	if( dst_fp == NULL )
	{
		fclose( src_fp );
		return -1;
	}

	fseek( src_fp, 0, SEEK_END );
	src_file_size = ftell( src_fp );
	rewind( src_fp );

	fprintf( dst_fp, "#ifndef %s\n", marconame_upper );
	fprintf( dst_fp, "#define %s\n", marconame_upper );
	fprintf( dst_fp, "\n" );
	fprintf( dst_fp, "const unsigned int %s_size = %ld;\n", marconame_lower, src_file_size );
	fprintf( dst_fp, "const char %s[] = {", marconame_lower );

	while( !feof( src_fp ) )
	{
		memset( buffer, 0, 2048 );
		read_size = fread( buffer, sizeof(char), 2048, src_fp );
		for( i=0; i<read_size; i++ )
		{
			if( (read_count+i)%16==0 )
			{
				fprintf( dst_fp, "\n\t" );
			}

			if( (read_count+i) >= src_file_size-1 )
			{
				fprintf( dst_fp, "0x%02X\n", buffer[i] );
				break;
			}
			else
			{
				fprintf( dst_fp, "0x%02X, ", buffer[i] );
			}
		}
		read_count += read_size;
	}

	fprintf( dst_fp, "};\n\n" );
	fprintf( dst_fp, "#endif\n");
	fflush( dst_fp );
	fclose( src_fp );
	fclose( dst_fp );
	return 0;
}

int main( int argc, char **argv )
{
	int  i = 0;
	if( argc==1 )
	{
		printf("Usage: %s image_files ... \n", argv[0] );
		return -1;
	}
	
	for( i = 1; i < argc; i++ )
	{
		if( _convert( argv[i] ) != 0 )
		{
			printf("CONVERT: %s failed!\n", argv[i] );
		}
	}

	return 0;
}

