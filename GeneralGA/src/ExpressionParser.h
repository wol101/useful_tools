// This class is used for parsing simple text math expressions

#ifndef ExpressionParser_h
#define ExpressionParser_h

#include <vector>
#include <string>

class ExpressionParserTokenGA;
class Genome;

enum ExpressionParserTokenTypeGA
{
    EPFunction = 0,
    EPOperator = 1,
    EPNumber = 2,
    EPVariable = 3,
    EPParser = 4
};

class ExpressionParserGA
{
public:
    ExpressionParserGA();
    ExpressionParserGA(ExpressionParserGA &parser);
    ~ExpressionParserGA();

    int CreateFromString(char *input, int length);
    double Evaluate(Genome *genome);
    
protected:

    static double EvaluateParser(ExpressionParserGA *parser, Genome *genome);
    static double EvaluateToken(ExpressionParserTokenGA *token, Genome *genome);
    static double EvaluateVariable(ExpressionParserTokenGA *token, Genome *genome);
    static double EvaluateFunction(ExpressionParserTokenGA *token, Genome *genome);
    static double EvaluateOperator(double lhs, double rhs, int index);    

    vector<ExpressionParserTokenGA *> m_ExpressionParserTokenList;
};

struct ExpressionParserTokenGA
{
public:
    ExpressionParserTokenGA() {value = -1;};
    ExpressionParserTokenGA(ExpressionParserTokenGA &parserToken)
    {
        type = parserToken.type;
        value = parserToken.value;
        index = parserToken.index;
        name = parserToken.name;
        ExpressionParserGA *p;
        for (unsigned int i = 0; i < parserToken.parserList.size(); i++)
        {
            p = new ExpressionParserGA(*parserToken.parserList[i]);
            parserList.push_back(p);
        }
    }
    ~ExpressionParserTokenGA()
    {
        for (unsigned int i = 0; i < parserList.size(); i++) delete parserList[i];
    }
    
    ExpressionParserTokenTypeGA type;
    double value;
    int index;
    string name;
    vector<ExpressionParserGA *> parserList;
};

#endif

