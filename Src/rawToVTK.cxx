// ############################################################################
//
// File:        rawToVTK.cpp
// Description: Converts a raw image file to 
//              VTK format (vtkStructuredPoints).
//              Use a vtkStructuredPointsReader to 
//              read the data.
// Date:        2006-04-05
// Author:      Erik Vidholm <erik@cb.uu.se>
//
// Compile instructions:
//
//   g++: g++ rawToVTK.cpp -o rawToVTK
//
// Example of usage:
//
//   $ rawToVTK myimage.raw myimage.vtk 256 256 256 1.0 1.0 2.5 BINARY
//
// ############################################################################
#include <iostream>
#include <fstream>
#include <typeinfo>
#include <string>
#include <cstdlib>
#include <string.h>

using namespace std;

// min
template<class T, class U>
T minOf(const T &a, const U &b) {
    return (a<b) ? a:b;
}

// is this architecture little endian
bool isLittleEndian() {
    union u {
        unsigned long l;
        unsigned char c[sizeof(unsigned long)];
    };
    u dummy;
    dummy.l = 1;
    return dummy.c[0] == 1;
}

// swap byte order
void swap2Bytes(unsigned char* &ptr) { 
    unsigned char tmp;
    tmp = ptr[0]; ptr[0] = ptr[1]; ptr[1] = tmp;
}
void swap4Bytes(unsigned char* &ptr) { 
    unsigned char tmp;
    tmp = ptr[0]; ptr[0] = ptr[3]; ptr[3] = tmp;
    tmp = ptr[1]; ptr[1] = ptr[2]; ptr[2] = tmp;
}
void swap8Bytes(unsigned char* &ptr) { 
    unsigned char tmp;
    tmp = ptr[0]; ptr[0] = ptr[7]; ptr[7] = tmp;
    tmp = ptr[1]; ptr[1] = ptr[6]; ptr[6] = tmp;
    tmp = ptr[2]; ptr[2] = ptr[5]; ptr[5] = tmp;
    tmp = ptr[3]; ptr[3] = ptr[4]; ptr[4] = tmp;
}

// swap byte order for an array
template<class T>
void byteSwapArray(T *arr, unsigned int arrlen) {
    int n = sizeof(T);
    unsigned char *p = (unsigned char*) arr;
    switch( n ) {
    case 1:
        return;
    case 2:
        for(unsigned int j=0; j<arrlen; j++, p+=n) {
            swap2Bytes(p);
        }
        break;
    case 4:
        for(unsigned int j=0; j<arrlen; j++, p+=n) {
            swap4Bytes(p);
        }
        break;
    case 8:
        for(unsigned int j=0; j<arrlen; j++, p+=n) {
            swap8Bytes(p);
        }
        break;
    default:
        break;
    }
}

// write binary with big endian byte order
// Here n is the number of elements, not the number of bytes
template<class T>
void writeBE(ostream &os, T *ptr, unsigned int n) {
    if( isLittleEndian() ) {
        // copy and write chunks of 1 MB = 1048576 bytes
        unsigned int chunkByteSize=1048576;
        unsigned int chunkElems=chunkByteSize/sizeof(T);
        unsigned int elemsWritten=0;
        T *cpy = new T[chunkElems];
        while( elemsWritten < n ) {
            chunkElems=minOf(n-elemsWritten,
                             (unsigned int) (chunkByteSize/sizeof(T)));
            memcpy(cpy, ptr+elemsWritten, chunkElems*sizeof(T));
            byteSwapArray(cpy, chunkElems);
            os.write((const char*) cpy, chunkElems*sizeof(T));
            elemsWritten += chunkElems;
        }
        delete [] cpy;
    }
    else {
        os.write((const char*) ptr, sizeof(T)*n);
    }
}

// the write function
template<class T>
bool writeVTK(T *ptr,                 // data pointer
              const string &filename, // filename
              int W, int H, int D,    // dimensions
              double sx = 1.0,        // spacing (voxelsize)
              double sy = 1.0,
              double sz = 1.0,
              const string &mode="BINARY") // binary/ascii
{ 

    ofstream os(filename.c_str(), ofstream::out | ofstream::binary);
    if( !os ) {
        cerr << "Unable to open file: " << filename << endl;
        return false;
    }

    os << "# vtk DataFile Version 3.0" << endl;
    os << "created by writeVTK" << endl;

    if( mode == "BINARY" || mode == "ASCII" )
        os << mode << endl;
    else {
        cerr << "Unknown mode!" << endl;
        return false;
    }

    os << "DATASET STRUCTURED_POINTS" << endl;
    os << "DIMENSIONS " << W << " " << H << " " << D << endl;
    os << "ORIGIN 0.0 0.0 0.0" << endl;
    os << "SPACING " << sx << " " << sy << " " << sz << endl;
    os << "POINT_DATA " << W*H*D << endl;

    string typestring;
    if( typeid(T) == typeid(unsigned char) )
        typestring = "unsigned_char";
    else if( typeid(T) == typeid(char) )
        typestring = "char";
    else if( typeid(T) == typeid(unsigned short) )
        typestring = "unsigned_short";
    else if( typeid(T) == typeid(short) )
        typestring = "short";
    else if( typeid(T) == typeid(unsigned int) )
        typestring = "unsigned_int";
    else if( typeid(T) == typeid(int) )
        typestring = "int";
    else if( typeid(T) == typeid(float) )
        typestring = "float";
    else if( typeid(T) == typeid(double) )
        typestring = "double";
    else {
        cerr << "Error: Can not write "
             << typeid(T).type_info::name() << endl;
        return false;
    }
    os << "SCALARS image_data " << typestring << " 1" << endl;
    os << "LOOKUP_TABLE default" << endl;

    if( mode == "BINARY" ) {
        writeBE( os, ptr, W*H*D );
    }
    else if( mode == "ASCII" ) {
        for(int z=0; z<D; z++) {
            for(int y=0; y<H; y++) {
                for(int x=0; x<W; x++) {
                    os << double(ptr[W*H*z+W*y+x]) << " ";
                }
                os << endl;
            }
        }
    }

    os.close();

    cout << "vtkStructuredPoints written successfully!" << endl;

    return true;
}

int main(int argc, char **argv) {

    if( argc < 10 ) {
        cerr << "Usage: " << argv[0] << " input.raw output.vtk W H D sx sy sz mode" << endl;
        exit(0);
    }

    // dimensions
    int W = atoi(argv[3]);
    int H = atoi(argv[4]);
    int D = atoi(argv[5]);
    float sx = atof(argv[6]);
    float sy = atof(argv[7]);
    float sz = atof(argv[8]);
    string mode(argv[9]);

    // allocate memory
    typedef float VOXELTYPE; // use your typedef here
    VOXELTYPE *image = new VOXELTYPE[W*H*D];

    // read raw data
    ifstream is(argv[1], ifstream::in | ifstream::binary);
    if( !is ) {
        cerr << "Can not open file!" << endl;
        exit(1);
    }
    is.read( (char *) image, W*H*D*sizeof(VOXELTYPE) );
    is.close();

    // write vtkStructuredPoints
    bool ok = writeVTK(image,
                       string(argv[2]),
                       W,H,D,
                       sx, sy, sz,
                       mode);

    // deallocate
    delete [] image;

    if( !ok ) {
        cerr << "Could not write file!" << endl;
        exit(1);
    }

    return 0;
}
