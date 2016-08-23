//#include "dcmtkfile.h"


//dcmtkfile::dcmtkfile()
//{

//}

//dcmtkfile::dcmtkfile(const char* path)
//{
//        DcmFileFormat fileformat;
//        OFCondition status = fileformat.loadFile(path);
//        if (status.good())
//        {
//            OFString patientName;
//            if(fileformat.getDataset()->findAndGetOFString(DCM_PatientName,patientName).good())
//            {
//                qDebug("%s\n",patientName);
//            }
//            else
//                cerr<<"Error access!"<<endl;
//        }
//        else cerr<<"Error open!"<<endl;
//}

