#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/mmvector.hpp"
#include "stdlib/string.hpp"
//#include "render/internal/trigroup.hpp"
#include "pertest/class1.hpp"
#include "machphys/ofactory.hpp"

//PER_DEFINE_PERSISTENT_T2( MachPhysObjectFactory, size_t,  string);
//PER_PRIVATE_READ_WRITE_PERSISTENT_T2(MachPhysObjectFactory, size_t,  string)
//PER_PRIVATE_DEFINE_PERSISTENT_T2( MachPhysObjectFactory , size_t,  string);


//void perWrite( PerOstream& ostr, const MachPhysObjectFactory< size_t, string> & factory )
//{
//   perWrite<size_t,string>( ostr, factory );
//}

int main(int argc, char *argv[])
{
    //MachPhysObjectFactory<size_t, string> factory(1);

     {
         std::ofstream    ostr( "test.bin", std::ios::binary );
         PerOstream  postr( ostr );

         string name = "Test case";
         postr << name;

         ctl_min_memory_vector< string* > mmv;
         string str("string in vec");
         mmv.push_back(&str);
         //postr << mmv;

         //ctl_min_memory_vector<RenITriangleGroup*> tris;
         //RenITriangleGroup grp(NULL);
         bool val = true;
         postr << val;

        ctl_min_memory_vector<Class1*> tris;
        Class1 dm;
        tris.push_back(&dm);
        postr << tris;
        //postr << factory;

         ostr.close();
         std::cout << "bool size: " << sizeof (val) << std::endl;
     }

     string name2;
     {
         std::ifstream    istr( "test.bin", std::ios::in | std::ios::binary );
         if (istr.good() && istr.is_open())
         {
             istr.seekg(0, std::ios::beg);
             PerIstream  pistr( istr );

             pistr >> name2;

            ctl_min_memory_vector< string* > mmv;
            //pistr >> mmv;
            //std::cout << *mmv[0];
            bool val;
            pistr >> val;

            ctl_min_memory_vector<Class1*> tris;
            pistr >> tris;

            istr.close();

            std::cout << name2 << std::endl;
            std::cout << "Bool value: " << val << std::endl;
         }
         else
            std::cout << "Unable to open file.\n";
     }
}

