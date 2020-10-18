
int main(int argc, char* argv[])
{
    std::ifstream fstr( "models.bin", std::ios::binary );
    PerIstream  istr( fstr );
    istr >> persistence_;
}
