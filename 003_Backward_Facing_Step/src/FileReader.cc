
#include "FileReader.hh"

#include <string>

//Register Integers in map
void FileReader::registerIntParameter(const std::string &key, int init)
{
   std::stringstream ss;
   ss << init;
   parameters[key] = ss.str();
}

//Register Double in Map
void FileReader::registerRealParameter(const std::string &key, real init)
{
   std::stringstream ss;
   ss << init;
   parameters[key] = ss.str();
}

//Register String in Map
void FileReader::registerStringParameter(const std::string &key, const std::string &init)
{
   parameters[key] = init;
}

//Set Parameters for String and String in Map
void FileReader::setParameter(const std::string &key, const std::string &in)
{
   CHECK_MSG(parameters.find(key) != parameters.end(), key+" not found in Input File" );
   PROGRESS (key+" MAPPED TO VARIABLE")
   parameters[key] = in;

}

//Set Parameters for String and Double in Map
void FileReader::setParameter(const std::string &key, real in)
{
   std::stringstream ss;
   ss << in;
   CHECK_MSG(parameters.find(key) != parameters.end(), key+" not found in Input File" );
   PROGRESS (key+" MAPPED TO VARIABLE")
   parameters[key] = ss.str();

}

//Set Parameters for String and Integer in Map
void FileReader::setParameter(const std::string &key, int in)
{
   std::stringstream ss;
   ss << in;
   CHECK_MSG(parameters.find(key) != parameters.end(), key+" not found in Input File" );
   PROGRESS (key+" MAPPED TO VARIABLE")
   parameters[key] = ss.str();

}

//Read File and Register and Set Parameters
bool FileReader::readFile(const std::string &name)
{
    PROGRESS ("Reading file "+name+" :")
   std::fstream fileInput (name,std::ios_base::in);
   std :: string readout, elem1, elem2;

   CHECK_MSG(fileInput.is_open(), "DATA FILE '" + name + "' NOT FOUND");

   while (fileInput.is_open())
   {
    while (std::getline(fileInput,readout))
    {
        std::istringstream ss(readout, std::istringstream::in);
        ss >> elem1 >> elem2;
        if(!elem1.empty() && !elem2.empty() && elem1[0] != '#' and elem2[0] != '#')
        {
         try
            {
               double i = std::stod(elem2);
               registerRealParameter(elem1,i);
               setParameter(elem1,i);

            }
            catch(...)
            {
                    registerStringParameter(elem1,elem2);
                    setParameter(elem1,elem2);
            }
        }

    }
    fileInput.close();
   }
   PROGRESS ("GIVEN DATA FILE READ AND ALL VALUES REGISTERED")

   return true;
}


//Print all parameters
void FileReader::printParameters() const
{
   for (auto it = parameters.begin(); it != parameters.end();it++)
   {
    std::cout << it -> first << '\t' << it -> second << std::endl;
   }
   PROGRESS ("All PARAMETERS in FileReader class Printed")
}

bool FileReader::findparameters(std::string param) const
{
    if (parameters.find(param) == parameters.end())
        return false;
    else
        return true;
}

std::string FileReader::findstring(std::string param) const
{
    if (parameters.find(param) == parameters.end())
        return "noslip";
    else
        return getStringParameter(param);
}
