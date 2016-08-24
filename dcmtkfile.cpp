#include "dcmtkfile.h"


dcmtkfile::dcmtkfile()
{

}

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
bool dcmtkfile::checkfile(QString path)
{
    DcmFileFormat fileformat;
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    QByteArray ba = path.toLocal8Bit();
    OFCondition status = fileformat.loadFile(ba.data());
    if (status.good())
    {
        if (fileformat.getDataset()->findAndGetOFString(DCM_PatientName,name).good())
        {

            qDebug("name: %s\n",name);
        }
        else
        {
            cerr<<"get name failed!"<<endl;
        }
        if (fileformat.getDataset()->findAndGetOFString(DCM_PatientID,id).good())
        {

            qDebug("id: %s\n",id);
        }
        else
        {
            cerr<<"get id failed!"<<endl;
        }
        if (fileformat.getDataset()->findAndGetOFString(DCM_StudyDate,date).good())
        {

            qDebug("date: %s\n",date);
        }
        else
        {
            cerr<<"get date failed!"<<endl;
        }
        if (fileformat.getDataset()->findAndGetOFString(DCM_StudyTime,time).good())
        {

            qDebug("time: %s\n",time);
        }
        else
        {
            cerr<<"get time failed!"<<endl;
        }
        if (fileformat.getDataset()->findAndGetUint16(DCM_Rows,height).good())
        {
            qDebug("height: %d\n",height);
        }
        else
        {
            cerr<<"get height failed!"<<endl;
        }
        if (fileformat.getDataset()->findAndGetUint16(DCM_Columns,width).good())
        {
            qDebug("width: %d\n",width);
        }
        else
        {
            cerr<<"get width failed!"<<endl;
        }
    }
    else
    {
        return false;
    }
}

cv::Mat dcmtkfile::loadfile(QString path)
{
    cv::Mat_<unsigned short> srcimgshort_temp;
    DcmFileFormat fileformat;
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    QByteArray ba = path.toLocal8Bit();
    OFCondition status = fileformat.loadFile(ba.data());
    if (status.good())
    {
        if (fileformat.getDataset()->findAndGetOFString(DCM_PatientName,name).good())
        {

            qDebug("name: %s\n",name);
        }
        else
        {
            cerr<<"get name failed!"<<endl;
        }
        if (fileformat.getDataset()->findAndGetOFString(DCM_PatientID,id).good())
        {

            qDebug("id: %s\n",id);
        }
        else
        {
            cerr<<"get id failed!"<<endl;
        }
        if (fileformat.getDataset()->findAndGetOFString(DCM_StudyDate,date).good())
        {

            qDebug("date: %s\n",date);
        }
        else
        {
            cerr<<"get date failed!"<<endl;
        }
        if (fileformat.getDataset()->findAndGetOFString(DCM_StudyTime,time).good())
        {

            qDebug("time: %s\n",time);
        }
        else
        {
            cerr<<"get time failed!"<<endl;
        }
        if (fileformat.getDataset()->findAndGetUint16(DCM_Rows,height).good())
        {
            qDebug("height: %d\n",height);
        }
        else
        {
            cerr<<"get height failed!"<<endl;
        }
        if (fileformat.getDataset()->findAndGetUint16(DCM_Columns,width).good())
        {
            qDebug("width: %d\n",width);
        }
        else
        {
            cerr<<"get width failed!"<<endl;
        }

        srcimgshort_temp=cv::Mat(height,width,CV_16UC1);
        const Uint16* pData = new Uint16[width*height];
        DcmDataset *dataset = fileformat.getDataset();
        if (dataset->findAndGetUint16Array(DCM_PixelData,pData).good())
        {
            for (int i=0;i<height;i++)
            {
                for (int j=0;j<width;j++)
                {
                    srcimgshort_temp(i,j)=65535 - pData[i*width+j];
                }
            }
        }
        else
        {
            cerr<<"read pixel failed!"<<endl;
        }
    }
    else
    {
        cerr<<"open error!"<<endl;
    }

    return srcimgshort_temp;

}
