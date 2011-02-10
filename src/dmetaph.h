////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
#include <QString>

class MString :  public QString
{

        int             length, last;
        const int     maxlength;
        bool    alternate;
        QString primary, secondary;

public:

        MString();
        MString(const char*, const int max);
        MString(const QString&, const int max);
        bool SlavoGermanic();
        bool IsVowel(int pos);
        inline void MetaphAdd(const char* main);
        inline void MetaphAdd(const char* main, const char* alt);
        bool StringAt(int start, int len, ... );
        void DoubleMetaphone(QString &metaph, QString &metaph2);
};
