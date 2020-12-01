# include <iostream>
# include <vector>
# include <ctype.h> //  ispunct(), isalpha(), isdigit()
# include <cstdlib> //  atof()
# include <stdio.h> //  printf()
# include <exception>
# include <sstream> //  stringstream-int2str()
# include <string>
// # define TRACE

using namespace std;

struct Token {
  string name;
  string category;
  int row;
  int column;
};

struct ID {
  string type;
  string name;
  int size;
  vector<string> array;
  string value;
  int layer;
};

struct Param {
  string type;
  string ref;
  string name;
  string size;
};

struct Function {
  string type;
  string name;
  vector<Param> formal_param_vec;
  vector<ID> locIDtable;
  vector<string> content;
};

Token gtoken;
vector<Token> gtoken_vec;
ID gid;
vector<ID> gID_table;
vector<Function> gFunc_table;
bool gIsQuit = false; // 處理error後quit(ID)的情況
bool gError1 = false;
int gLine = 0;
bool ginFuncDef = false;
Function gFuncDef;
int gCompoundLayer = 0;
bool gCoutState = false;
bool gCondTRUE = true;
vector<string> gCondResult_vec;
bool gPPMM_after = false;
string gPPMM_IDname;
bool gPPMM_isArray = false;
string gPPMM_result;
int gPPMM_IDindex = 0;
bool ginLocal = false;
vector<ID> gLocal_table;
vector<bool> gAssignable_vec; // used by ( if, else, while )

void Error2( int &cur ) ;
void Type_error( string correct_type, string result, string type ) ;
void Init_gFuncDef() ;
bool AllisNum( string str ) ;
bool AllisFloat( string str ) ;
bool AllisString( string str ) ;
bool AllisChar( string str ) ;
bool AllisID( string str ) ;
bool IsInt( string str ) ;
bool IsFloat( string str ) ;
string Type( string result ) ;
string Get_str_or_char( string str ) ;
double Num( string result, string type ) ;
string Float2str( double d ) ;
string Int2str( int d ) ;
void Make_Func_locIDtable( vector<ID> tempid_vec ) ;
void Make_locID_table( vector<ID> tempid_vec ) ;
void Local_table_pop() ;
void Make_ID_table( vector<ID> tempid_vec ) ;
void Make_func_table() ;
Function FindFunc( string id ) ;
ID FindID( string id ) ;
void Check_ID_exist( string id_name ) ;
void Assign_IDarray( string id_name, int index, string result ) ;
void Assign_ID( string id_name, string result ) ;
bool Assignable() ;
bool Cond1_Assignable() ;
bool Cond2_Assignable() ;
void Do_function( string id_name, vector<string> param_vec, string &result ) ;
void Process( string &str, string line, int i ) ;
void Make_token( string &code ) ;
void Token_classify() ;
string LastToken( int cur ) ;
string GetToken( int &cur ) ;
string PeekToken( int cur ) ;
void User_input( int &cur ) ;
void Definition( int &cur ) ;
bool Type_specifier( string type ) ;
void Function_definition_or_declarators( int &cur, string type, string id ) ;
void Rest_of_declarators( int &cur, string type, string id ) ;
void Function_definition_without_ID( int &cur, string func_type, string func_id ) ;
void Formal_parameter_list( int &cur, vector<Param> &params ) ;
void Compound_statement( int &cur ) ;
void Declaration( int &cur ) ;
void Statement( int &cur ) ;
vector<string> Expression( int &cur, string &result, string &type ) ;
string Basic_expression( int &cur, string &result, string &type ) ;
void Rest_of_ISBE( int &cur, string id, string &result, string &type ) ;
void Rest_of_PPMM_ISBE( int &cur, string ppmm, string id_name, string &result, string &type ) ;
bool Sign( string sign ) ;
void Actual_parameter_list( int &cur, vector<string> &param_vec ) ;
bool Assignment_operator( string op ) ;
void Romce_and_romloe( int &cur, string &result, string &type ) ;
void Rest_of_maybe_logical_OR_exp( int &cur, string &result, string &type ) ;
void Maybe_logical_AND_exp( int &cur, string &result, string &type ) ;
void Rest_of_maybe_logical_AND_exp( int &cur, string &result, string &type ) ;
void Maybe_bit_OR_exp( int &cur, string &result, string &type ) ;
void Rest_of_maybe_bit_OR_exp( int &cur, string &result, string &type ) ;
void Maybe_bit_ex_OR_exp( int &cur, string &result, string &type ) ;
void Rest_of_maybe_bit_ex_OR_exp( int &cur, string &result, string &type ) ;
void Maybe_bit_AND_exp( int &cur, string &result, string &type ) ;
void Rest_of_maybe_bit_AND_exp( int &cur, string &result, string &type ) ;
void Maybe_equality_exp( int &cur, string &result, string &type ) ;
void Rest_of_maybe_equality_exp( int &cur, string &result, string &type ) ;
void Maybe_relational_exp( int &cur, string &result, string &type ) ;
void Rest_of_maybe_relational_exp( int &cur, string &result, string &type ) ;
void Maybe_shift_exp( int &cur, string &result, string &type ) ;
void Rest_of_maybe_shift_exp( int &cur, string &result, string &type ) ;
void Maybe_additive_exp( int &cur, string &result, string &type ) ;
void Rest_of_maybe_additive_exp( int &cur, string &result, string &type ) ;
void Maybe_mult_exp( int &cur, string &result, string &type ) ;
void Rest_of_maybe_mult_exp( int &cur, string &result, string &type ) ;
void Unary_exp( int &cur, string &result, string &type ) ;
void Signed_unary_exp( int &cur, string &result, string &type, string sign ) ;
void Unsigned_unary_exp( int &cur, string &result, string &type ) ;



void Error2( int &cur ) {
  string token = GetToken( cur );
  string str = "Line " + Int2str( gLine ) + " : unexpected token : '" + token + "'";
  throw str.c_str();
} // Error2()

void Type_error( string correct_type, string result, string type ) {
  string str = "correct type is: " + correct_type + ", but(" + result + "," + type + ")";
  throw str.c_str();
} // Type_error()

void Init_gFuncDef() {
  gFuncDef.type = "";
  gFuncDef.name = "";
  gFuncDef.formal_param_vec.clear();
  gFuncDef.locIDtable.clear();
  gFuncDef.content.clear();
} // Init_gFuncDef()

bool AllisNum( string str ) {
  for ( int i = 0; i < str.size() ; i++ )
  {  
    int temp = ( int ) str[i];
    if ( temp >= 48 && temp <= 57 )
    {
      ;
    } // if
    else
    {
      return false;
    } // else
  } // for ( str )
  
  return true;
} // AllisNum()

bool AllisFloat( string str ) {
  int dot = 0;
  for ( int i = 0; i < str.size() ; i++ )
  {
    int temp = ( int ) str[i];
    if ( temp == 46 )
    {
      dot++;
    } // if
    else if ( temp < 48 || temp > 57 )
    {
      return false;
    } // else if
  } // for ( str )
  
  if ( dot != 1 )
  {
    return false;
  } // if
  else return true;
} // AllisFloat()

bool AllisString( string str ) { // "string"
  if ( str[0] == '"' && str[str.size()-1] == '"' )
  {
    return true;
  } // if
  else return false;
} // AllisString() 

bool AllisChar( string str ) { // 'c'
  if ( str[0] == '\'' && str[str.size()-1] == '\'' )
  {
    return true;
  } // if
  else return false;
} // AllisChar()

bool AllisID( string str ) {
  
  if ( !isalpha( str[0] ) ) { return false; }
  
  for ( int i = 1; i < str.size() ; i++ )
  {  
    int temp = ( int ) str[i];
    if ( ( temp >= 48 && temp <= 57 ) || ( temp >= 65 && temp <= 90 )
         || ( temp >= 97 && temp <= 122 ) || str[i] == '_' ) // digit & alpha
    {
      ;
    } // if
    else
    {
      return false;
    } // else
  } // for ( str )
  
  return true;
} // AllisID()

bool IsInt( string str ) { // for Type(), include '-'
  for ( int i = 0; i < str.size() ; i++ )
  {  
    int temp = ( int ) str[i];
    if ( temp >= 48 && temp <= 57 )
    {
      ;
    } // if
    else if ( i == 0 && str[0] == '-' )
    {
      ;
    } // else if
    else
    {
      return false;
    } // else
  } // for ( str )
  
  return true;
} // IsInt()

bool IsFloat( string str ) { // for Type(), include '-'
  int dot = 0;
  for ( int i = 0; i < str.size() ; i++ )
  {
    int temp = ( int ) str[i];
    if ( temp == 46 )
    {
      dot++;
    } // if
    else if ( i == 0 && str[0] == '-' )
    {
      ;
    } // else if
    else if ( temp < 48 || temp > 57 )
    {
      return false;
    } // else if
  } // for ( str )
  
  if ( dot != 1 )
  {
    return false;
  } // if
  else return true;
} // IsFloat()

string Type( string result ) {
  string type;
  
  if ( IsInt( result ) )
  {
    type = "int";
  } // if int
  else if ( IsFloat( result ) )
  {
    type = "float";
  } // else if float
  else if ( AllisString( result ) )
  {
    type = "string";
  } // else if string
  else if ( AllisChar( result ) )
  {
    type = "char";
  } // else if char
  else if ( result == "true" || result == "false" )
  {
    type = "bool";
  } // else if bool
  
  return type;
} // Type()

string Get_str_or_char( string str ) {
  // "string" -> string (去頭去尾)
  //      'c' -> c      (去頭去尾) 
  string find_name;

  if ( str[0] != '\"' && str[0] != '\'' )
  {
    string err = "Line " + Int2str( gLine ) + " : unexpected token : '" + str + "'";
    throw err.c_str();
  } // if

  for ( int i = 1 ; i < str.size()-1 ; i++ )
  {
    find_name += str[i];
  } // for

  return find_name;
} // Get_str_or_char()

double Num( string result, string type ) {
  if ( type == "int" || type == "float" )
  {
    return atof( result.c_str() );
  } // if
  else if ( type == "char" )
  {
    // 'c' -> 'c' (去頭去尾)
    string ch = Get_str_or_char( result );
    char c = ch[0];
    double f = ( float ) ( c );
    
    return f;
  } // else if
  else if ( type == "bool" )
  {
    if ( result == "true" )
    {
      return 1;
    } // if
    else if ( result == "false" )
    {
      return 0;
    } // else if
  } // else if
  else { Type_error( "int, flaot, char, bool", result, type ); } // else

  return 0;
} // Num()

string Float2str( double d ) {
  stringstream ss;
  ss << fixed << d;
  string str = ss.str();
  
  // erase trailing zero
  for ( int i = str.size() - 1 ; i > 0 ; i-- )
  {
    if ( str[i] == '0' )
    {
      str.erase( i, 1 );
    } // if
    else if ( str[i] == '.' )
    {
      str.erase( i, 1 );
      return str;
    } // else if
    else
    {
      return str;
    } // else
  } // for
  
  return str;
} // Float2str()

string Int2str( int d )
{
  stringstream ss;
  ss << d;
  
  return ss.str();
} // Int2str()

void Make_Func_locIDtable( vector<ID> tempid_vec ) {

  for ( int i = 0 ; i < tempid_vec.size() ; i++ )
  {
    bool id_defined = false;

    for ( int j = 0 ; j < gFuncDef.locIDtable.size() ; j++ )
    {
      if ( tempid_vec[i].name == gFuncDef.locIDtable[j].name )
      {
        id_defined = true;
        gFuncDef.locIDtable[j] = tempid_vec[i];
      } // if
    } // for new definition
    
    if ( id_defined == false )
    {
      gFuncDef.locIDtable.push_back( tempid_vec[i] );
    } // if definition 
  } // for

  /*
  // (debug)
  cout << gFuncDef;
  for (int i = 0 ; i < gFuncDef.locIDtable.size() ; i++ ) 
  {
    cout << "{" << gFuncDef.locIDtable[i].name << "}";
  }
  */
} // Make_Func_locIDtable()

void Make_locID_table( vector<ID> tempid_vec ) {
  
  for ( int i = 0 ; i < tempid_vec.size() ; i++ )
  {
    bool id_defined = false;

    for ( int j = 0 ; j < gLocal_table.size() ; j++ )
    {
      if ( tempid_vec[i].name == gLocal_table[j].name &&
           tempid_vec[i].layer == gLocal_table[j].layer )
      {
        id_defined = true;
        gLocal_table[j] = tempid_vec[i];
      } // if
    } // for new definition
    
    if ( id_defined == false )
    {
      gLocal_table.push_back( tempid_vec[i] );
    } // if definition 
  } // for

  /*
  // (debug)
  for (int i = 0 ; i < gLocal_table.size() ; i++ ) 
  {
    cout << "{" << gLocal_table[i].name << gLocal_table[i].layer << "}";
  }
  */
} // Make_locID_table()

void Local_table_pop() {
  for ( int i = 0 ; i < gLocal_table.size() ; i++ )
  {
    if ( gLocal_table[i].layer == gCompoundLayer )
    {
      gLocal_table.erase( gLocal_table.begin() + i );
    } // if
  } // for
} // Local_table_pop()

void Make_ID_table( vector<ID> tempid_vec ) {

  for ( int i = 0 ; i < tempid_vec.size() ; i++ )
  {
    bool id_defined = false;

    if ( i > 0 )
    {
      cout << endl;
    } // if
    
    for ( int j = 0 ; j < gID_table.size() ; j++ )
    {
      if ( tempid_vec[i].name == gID_table[j].name )
      {
        id_defined = true;
        gID_table[j] = tempid_vec[i];
        cout << "New definition of " << tempid_vec[i].name << " entered ...";
      } // if
    } // for
    
    if ( id_defined == false )
    {
      gID_table.push_back( tempid_vec[i] );
      cout << "Definition of " << tempid_vec[i].name << " entered ..." ;
    } // if
  } // for
  
  /*
  for (int i = 0; i < gID_table.size(); i++) // (debug)
  {
    cout << "[" << gID_table[i].name << gID_table[i].value << gID_table[i].type << "]";
  }
  */
} // Make_ID_table()

void Make_func_table() {
  bool func_defined = false;
  
  for ( int i = 0 ; i < gFunc_table.size() ; i++ )
  {
    if ( gFuncDef.name == gFunc_table[i].name )
    {
      func_defined = true;
      gFunc_table[i] = gFuncDef;
      cout << "New definition of " << gFuncDef.name << "()" << " entered ...";
    } // if
  } // for
  
  if ( func_defined == false )
  {
    gFunc_table.push_back( gFuncDef );
    cout << "Definition of " << gFuncDef.name << "()" << " entered ...";
  } // if
  
  /*
  for (int i = 0; i < gFunc_table.size(); i++) // (debug)
  {
    cout << "[" << gFunc_table[i].name << gFunc_table[i].value << "]";
  }
  */ 
} // Make_func_table()

ID FindID( string id ) {
  if ( ginLocal )
  {
    ID deepest_local; // used to return the var which has deepest(biggest) layer
    bool have_local = false;

    for ( int i = 0 ; i < gLocal_table.size() ; i++ )
    {
      if ( id == gLocal_table[i].name && gLocal_table[i].layer > deepest_local.layer )
      {
        have_local = true;
        deepest_local = gLocal_table[i];
      } // if
    } // for
    
    if ( have_local ) { return deepest_local; } // if
  } // if

  for ( int i = 0 ; i < gID_table.size() ; i++ )
  {
    if ( id == gID_table[i].name )
    {
      return gID_table[i];
    } // if
  } // for

  return gID_table[0]; // 希望不要走到這
} // FindID()

Function FindFunc( string id ) {
  for ( int i = 0 ; i < gFunc_table.size() ; i++ )
  {
    if ( id == gFunc_table[i].name )
    {
      return gFunc_table[i];
    } // if
  } // for

  return gFunc_table[0]; // 希望不要走到這 
} // FindFunc()

void Check_ID_exist( string id_name ) {
  
  if ( ginLocal )
  {
    for ( int i = 0 ; i < gLocal_table.size() ; i++ )
    {
      if ( id_name == gLocal_table[i].name )
      {
        return;
      } // if
    } // for
  } // if ginLocal
  
  if ( ginFuncDef )
  {
    for ( int i = 0 ; i < gFuncDef.formal_param_vec.size() ; i++ )
    {
      if ( id_name == gFuncDef.formal_param_vec[i].name )
      {
        return; 
      } // if
    } // for 

    for ( int i = 0 ; i < gFuncDef.locIDtable.size() ; i++ )
    {
      if ( id_name == gFuncDef.locIDtable[i].name )
      {
        return; 
      } // if
    } // for 
  } // if in function define phase
  
  if ( id_name == "Done" ) { return; } // if

  if ( id_name == "ListAllVariables" ) { return; } // if

  if ( id_name == "ListAllFunctions" ) { return; } // if

  if ( id_name == "ListVariable" ) { return; } // if

  if ( id_name == "ListFunction" ) { return; } // if

  for ( int i = 0 ; i < gID_table.size() ; i++ )
  {
    if ( id_name == gID_table[i].name )
    {
      return; 
    } // if
  } // for
  
  for ( int i = 0 ; i < gFunc_table.size() ; i++ )
  {
    if ( id_name == gFunc_table[i].name )
    {
      return; 
    } // if
  } // for
  
  // error3
  string str = "Line " + Int2str( gLine ) + " : undefined identifier : '" + id_name + "'";
  throw str.c_str();
} // Check_ID_exist()

void Assign_IDarray( string id_name, int index, string result ) {
  bool have_local = false;
  if ( ginLocal )
  {
    int tmp_layer = 0;
    int loc_index = 0;
    
    for ( int i = 0 ; i < gLocal_table.size() ; i++ )
    {
      if ( id_name == gLocal_table[i].name && gLocal_table[i].layer > tmp_layer )
      {
        have_local = true;
        tmp_layer = gLocal_table[i].layer;
        loc_index = i;
      } // if
    } // for
    
    if ( have_local && gCondTRUE == true ) 
    {
      gLocal_table[loc_index].array[index] = result;
    } // if
  } // if ginLocal

  if ( have_local == false )
  {
    for ( int i = 0 ; i < gID_table.size() ; i++ )
    {
      if ( id_name == gID_table[i].name )
      {
        if ( gCondTRUE == true )
        {
          gID_table[i].array[index] = result;
        } // if not in condition exp
      } // if
    } // for
  } // if have_local == false
} // Assign_IDarray()

void Assign_ID( string id_name, string result ) {
  bool have_local = false;
  if ( ginLocal )
  {
    int tmp_layer = 0;
    int loc_index = 0;
    
    for ( int i = 0 ; i < gLocal_table.size() ; i++ )
    {
      if ( id_name == gLocal_table[i].name && gLocal_table[i].layer > tmp_layer )
      {
        have_local = true;
        tmp_layer = gLocal_table[i].layer;
        loc_index = i;
      } // if
    } // for
    
    if ( have_local && gCondTRUE == true ) 
    {
      gLocal_table[loc_index].value = result;
    } // if
  } // if ginLocal

  if ( have_local == false )
  {
    for ( int i = 0 ; i < gID_table.size() ; i++ )
    {
      if ( id_name == gID_table[i].name )
      {
        if ( gCondTRUE == true )
        {
          gID_table[i].value = result;
        } // if not in condition exp
      } // if
    } // for
  } // if have_local == false
} // Assign_ID()

bool Assignable() {
  for ( int i = 0 ; i < gAssignable_vec.size() ; i++ )
  {
    if ( gAssignable_vec[i] == false )
    {
      return false;
    } // if
  } // for

  return true;
} // Assignable()

bool Cond1_Assignable() {
  for ( int i = 0 ; i < gCondResult_vec.size() ; i++ )
  {
    if ( gCondResult_vec[i] == "false" || gCondResult_vec[i] == "0" )
    {
      return false;
    } // if
  } // for
  
  return true;
} // Cond1_Assignable()

bool Cond2_Assignable() {
  for ( int i = 0 ; i < gCondResult_vec.size() ; i++ )
  {
    if ( gCondResult_vec[i] == "true" || gCondResult_vec[i] == "1" )
    {
      return false;
    } // if
  } // for
  
  return true;
} // Cond2_Assignable()

void Do_function( string id_name, vector<string> param_vec, string &result ) {
  
  // System function
  if ( id_name == "Done" )
  {
    gIsQuit = true;
  } // if Done()
  else if ( id_name == "ListAllVariables" )
  {
    // Sort Variables by name
    for ( int i = 0 ; i < gID_table.size() - 1 ; i++ )
    {
      for ( int j = i + 1 ; j < gID_table.size() ; j++ )
      {
        int name_i = 0;
        while ( gID_table[i].name[name_i] == gID_table[j].name[name_i] )
        {
          name_i++; // 前面字母如果一樣就要比較後面的 
        } // while
        
        if ( gID_table[i].name[name_i] > gID_table[j].name[name_i] )
        {
          ID temp;
          temp = gID_table[i];
          gID_table[i] = gID_table[j];
          gID_table[j] = temp;
        } // if
      } // for
    } // for Sort Variables by name
    
    // print all Variables
    for ( int i = 0 ; i < gID_table.size() ; i++ )
    {
      cout << gID_table[i].name << endl;
    } // for
  } // else if ListAllVariables()
  else if ( id_name == "ListAllFunctions" )
  {
    // Sort function by name
    for ( int i = 0 ; i < gFunc_table.size() - 1 ; i++ )
    {
      for ( int j = i + 1 ; j < gFunc_table.size() ; j++ )
      {
        int name_i = 0;
        while ( gFunc_table[i].name[name_i] == gFunc_table[j].name[name_i] )
        {
          name_i++; // 前面字母如果一樣就要比較後面的 
        } // while
        
        if ( gFunc_table[i].name[name_i] > gFunc_table[j].name[name_i] )
        {
          Function temp;
          temp = gFunc_table[i];
          gFunc_table[i] = gFunc_table[j];
          gFunc_table[j] = temp;
        } // if
      } // for
    } // for Sort function by name
    
    // print all function
    for ( int i = 0 ; i < gFunc_table.size() ; i++ )
    {
      cout << gFunc_table[i].name << "()" << endl;
    } // for
  } // else if ListAllIFunctions()
  else if ( id_name == "ListVariable" )
  {
    // "string" -> string (去頭去尾)
    string find_name = Get_str_or_char( param_vec[0] );
    
    // print
    ID id = FindID( find_name );

    cout << id.type << " " << id.name;

    if ( id.size != -1 )
    {
      cout << "[ " << id.size << " ]";
    } // if
    
    cout << " ;" << endl;
  } // else if ListVariable()
  else if ( id_name == "ListFunction" )
  {
    // "string" -> string (去頭去尾)
    string find_name = Get_str_or_char( param_vec[0] );
    
    for ( int i = 0 ; i < gFunc_table.size() ; i++ )
    {
      if ( find_name == gFunc_table[i].name )
      {
        cout << gFunc_table[i].type << " " << gFunc_table[i].name << "( " ;
        
        for ( int j = 0 ; j < gFunc_table[i].formal_param_vec.size() ; j++ )
        {
          if ( j > 0 )
          {
            cout << ", ";
          } // if not the first parameter

          Param param = gFunc_table[i].formal_param_vec[j];
          
          cout << param.type << " ";

          if ( !param.ref.empty() )
          {
            cout << "&";
          } // if param have&
          
          cout << param.name;
          
          if ( !param.size.empty() )
          {
            cout << "[ " << param.size << " ]";
          } // if param have [size]
        } // for print param_vec
        
        cout << " ) {" << endl;
        
        int compound_layer = 1;
        for ( int j = 0 ; j < gFunc_table[i].content.size() ; j++ )
        {
          if ( j == 0 )
          {
            cout << "  ";
          } // if
          
          if ( j < gFunc_table[i].content.size() - 1 &&
               ( gFunc_table[i].content[j+1] == "[" ||
                 gFunc_table[i].content[j+1] == "(" ||
                 gFunc_table[i].content[j+1] == "++" ||
                 gFunc_table[i].content[j+1] == "--" ) &&
               ( gFunc_table[i].content[j] != "if" &&
                 gFunc_table[i].content[j] != "else" &&
                 gFunc_table[i].content[j] != "while" &&
                 gFunc_table[i].content[j] != "do" ) )
          {
            cout << gFunc_table[i].content[j];
          } // if
          else
          {
            if ( gFunc_table[i].content[j] == ";" ||
                 gFunc_table[i].content[j] == "}" ||
                 gFunc_table[i].content[j] == "{" )
            {
              cout << gFunc_table[i].content[j] << endl;
              
              if ( gFunc_table[i].content[j] == "{" )
              {
                compound_layer++;
              } // if
              
              if ( j < gFunc_table[i].content.size() - 1 &&
                   gFunc_table[i].content[j+1] == "}" )
              {
                compound_layer--;
              } // if
              
              if ( j < gFunc_table[i].content.size() - 1 )
              {
                for ( int k = 0 ; k < compound_layer * 2 ; k++ )
                {
                  cout << " ";
                } // for
              } // if
            } // if
            else
            {
              cout << gFunc_table[i].content[j] << " ";
            } // else
          } // else
        } // for print content
        
        cout << "}" << endl;
      } // if
    } // for
  } // else if ListFunction()
  
  // user function
  // result在這邊改 
} // Do_function()

void Process( string &str, string line, int i )
{
  if ( !str.empty() )
  {
    gtoken.name = str;
    gtoken_vec.push_back( gtoken );
  } // if

  gtoken.name = line[i];
  gtoken_vec.push_back( gtoken );
  str = "";
} // Process()

void Make_token( string &code ) {
  string line;
  string str; 
  for ( int i = 0 ; i < code.size() ; i++ )
  {
    line += code[i];
    if ( code[i] == '\n' )
    {
      bool isString = false;
      bool isChar = false;
      for ( int j = 0 ; j < line.size() ; j++ )
      {
        //  cout << "[" <<line[j] << "]"; (debug)
        if ( isString == true ) // in "string"
        {
          if ( line[j] == '"' )
          {
            str = str.append( line, j, 1 );
            gtoken.name = str;
            gtoken_vec.push_back( gtoken );
            str = "";
            isString = false;
          } // if
          else
          {
            str = str.append( line, j, 1 );
          } // else
        } // if
        else if ( isChar == true )
        {
          if ( line[j] == '\'' )
          {
            str = str.append( line, j-1, 2 ); // '和'前一個字符加進來 
            gtoken.name = str; // 'c' 
            gtoken_vec.push_back( gtoken );
            str = "";
            isChar = false;
          } // if
        } // else if
        else // string外,正常情況 
        {
          if ( line[j] == ' ' || line[j] == '\t' )
          {
            if ( !str.empty() )
            {
              gtoken.name = str;
              gtoken_vec.push_back( gtoken );
              str = "";
            } // if
          } // if
          else if ( line[j] == '\n' ) { Process( str, line, j ); } // else if
          else if ( line[j] == '^' ) { Process( str, line, j ); } // else if
          else if ( line[j] == '(' ) { Process( str, line, j ); } // else if
          else if ( line[j] == ')' ) { Process( str, line, j ); } // else if
          else if ( line[j] == '[' ) { Process( str, line, j ); } // else if
          else if ( line[j] == ']' ) { Process( str, line, j ); } // else if
          else if ( line[j] == '{' ) { Process( str, line, j ); } // else if
          else if ( line[j] == '}' ) { Process( str, line, j ); } // else if
          else if ( line[j] == ';' ) { Process( str, line, j ); } // else if
          else if ( line[j] == ',' ) { Process( str, line, j ); } // else if
          else if ( line[j] == '?' ) { Process( str, line, j ); } // else if
          else if ( line[j] == '&' ) // 
          {
            if ( line[j+1] == '&' )
            {
              if ( !str.empty() )
              {
                gtoken.name = str;
                gtoken_vec.push_back( gtoken );
                str = "";
              } // if
              
              str = str.append( line, j, 1 );
            } // if
            else if ( line[j-1] == '&' )
            {
              str = str.append( line, j, 1 );
              gtoken.name = str;
              gtoken_vec.push_back( gtoken );
              str = "";
            } // else if
            else // 單個'&' 
            {
              Process( str, line, j );
            } // else
          } // else if '&' 
          else if ( line[j] == '|' ) //
          {
            if ( line[j+1] == '|' )
            {
              if ( !str.empty() )
              {
                gtoken.name = str;
                gtoken_vec.push_back( gtoken );
                str = "";
              } // if
              
              str = str.append( line, j, 1 );
            } // if
            else if ( line[j-1] == '|' )
            {
              str = str.append( line, j, 1 );
              gtoken.name = str;
              gtoken_vec.push_back( gtoken );
              str = "";
            } // else if
            else // 單個'|' 
            {
              Process( str, line, j );
            } // else
          } // else if '|'
          else if ( line[j] == '<' ) //
          {
            if ( line[j+1] == '=' || line[j+1] == '<' )
            {
              if ( !str.empty() )
              {
                gtoken.name = str;
                gtoken_vec.push_back( gtoken );
                str = "";
              } // if
              
              str = str.append( line, j, 1 );
            } // if
            else if ( line[j-1] == '<' )
            {
              str = str.append( line, j, 1 );
              gtoken.name = str;
              gtoken_vec.push_back( gtoken );
              str = "";
            } // else if
            else // 單個'<' 
            {
              Process( str, line, j );
            } // else
          } // else if '<'
          else if ( line[j] == '>' ) //
          {
            if ( line[j+1] == '=' || line[j+1] == '>' )
            {
              if ( !str.empty() )
              {
                gtoken.name = str;
                gtoken_vec.push_back( gtoken );
                str = "";
              } // if
              
              str = str.append( line, j, 1 );
            } // if
            else if ( line[j-1] == '>' )
            {
              str = str.append( line, j, 1 );
              gtoken.name = str;
              gtoken_vec.push_back( gtoken );
              str = "";
            } // else if
            else // 單個'>' 
            {
              Process( str, line, j );
            } // else
          } // else if '>'
          else if ( line[j] == '!' ) //
          {
            if ( line[j+1] == '=' )
            {
              if ( !str.empty() )
              {
                gtoken.name = str;
                gtoken_vec.push_back( gtoken );
                str = "";
              } // if
              
              str = str.append( line, j, 1 );
            } // if
            else // 單個'!' 
            {
              Process( str, line, j );
            } // else
          } // else if '!'
          else if ( line[j] == '+' ) //
          {
            if ( line[j+1] == '=' || line[j+1] == '+' )
            {
              if ( !str.empty() )
              {
                gtoken.name = str;
                gtoken_vec.push_back( gtoken );
                str = "";
              } // if
              
              str = str.append( line, j, 1 );
            } // if
            else if ( line[j-1] == '+' )
            {
              str = str.append( line, j, 1 );
              gtoken.name = str;
              gtoken_vec.push_back( gtoken );
              str = "";
            } // else if
            else // 單個'+' 
            {
              Process( str, line, j );
            } // else
          } // else if '+'
          else if ( line[j] == '-' ) //
          {
            if ( line[j+1] == '=' || line[j+1] == '-' )
            {
              if ( !str.empty() )
              {
                gtoken.name = str;
                gtoken_vec.push_back( gtoken );
                str = "";
              } // if
              
              str = str.append( line, j, 1 );
            } // if
            else if ( line[j-1] == '-' )
            {
              str = str.append( line, j, 1 );
              gtoken.name = str;
              gtoken_vec.push_back( gtoken );
              str = "";
            } // else if
            else // 單個'-' 
            {
              Process( str, line, j );
            } // else
          } // else if '-'
          else if ( line[j] == '*' ) //
          {
            if ( line[j+1] == '=' )
            {
              if ( !str.empty() )
              {
                gtoken.name = str;
                gtoken_vec.push_back( gtoken );
                str = "";
              } // if
              
              str = str.append( line, j, 1 );
            } // if
            else // 單個'*' 
            {
              Process( str, line, j );
            } // else
          } // else if '*'
          else if ( line[j] == '/' ) //
          {
            if ( line[j+1] == '/' ) // 碰到註解 
            {
              j = line.size() - 2; // 將 j 指定到'\n'前 
            } // if
            else if ( line[j+1] == '=' )
            {
              if ( !str.empty() )
              {
                gtoken.name = str;
                gtoken_vec.push_back( gtoken );
                str = "";
              } // if
              
              str = str.append( line, j, 1 );
            } // else if
            else // 單個'/' 
            {
              Process( str, line, j );
            } // else
          } // else if '/'
          else if ( line[j] == '%' ) //
          {
            if ( line[j+1] == '=' )
            {
              if ( !str.empty() )
              {
                gtoken.name = str;
                gtoken_vec.push_back( gtoken );
                str = "";
              } // if
              
              str = str.append( line, j, 1 );
            } // if
            else // 單個'%' 
            {
              Process( str, line, j );
            } // else
          } // else if '%'
          else if ( line[j] == '=' ) //
          {
            if ( line[j+1] == '=' )
            {
              if ( !str.empty() )
              {
                gtoken.name = str;
                gtoken_vec.push_back( gtoken );
                str = "";
              } // if
              
              str = str.append( line, j, 1 );
            } // if
            else if ( line[j-1] == '<' || line[j-1] == '>' ||
                      line[j-1] == '!' || line[j-1] == '=' ||
                      line[j-1] == '+' || line[j-1] == '-' ||
                      line[j-1] == '*' || line[j-1] == '/' ||
                      line[j-1] == '%' )
            {
              str = str.append( line, j, 1 );
              gtoken.name = str;
              gtoken_vec.push_back( gtoken );
              str = "";
            } // else if
            else
            {
              Process( str, line, j );
            } // else
          } // else if '='
          else if ( line[j] == '"' ) //
          {
            isString = true;
            if ( !str.empty() )
            {
              gtoken.name = str;
              gtoken_vec.push_back( gtoken );
              str = "";
            } // if
            
            str = str.append( line, j, 1 );
          } // else if
          else if ( line[j] == '\'' )
          {
            isChar = true;
            if ( !str.empty() )
            {
              gtoken.name = str;
              gtoken_vec.push_back( gtoken );
              str = "";
            } // if
            
            str = str.append( line, j, 1 );
          } // else if
          else if ( line[j] == '_' ) // 算在ID裡 
          {
            if ( AllisID( str ) )
            {
              str = str.append( line, j, 1 );
            } // if
            else
            {
              Process( str, line, j ); // 非法符號
            } // else
          } // else if
          else if ( line[j] == '.' ) //
          {
            if ( AllisNum( str ) )
            {
              str = str.append( line, j, 1 );
            } // if
            else if ( isdigit( line[j+1] ) )
            {
              if ( !str.empty() )
              {
                gtoken.name = str;
                gtoken_vec.push_back( gtoken );
                str = "";
              } // if
              
              str = str.append( line, j, 1 );
            } // else if
            else // 單個'.'(非法字元) 
            {
              Process( str, line, j );
            } // else
          } // else if '.'
          else if ( isdigit( line[j] ) ) // 數字 
          {
            str = str.append( line, j, 1 );
          } // else if
          else if ( isalpha( line[j] ) ) // 字母 
          {
            if ( !str.empty() ) // 處理字母前全為數字的情況　例：３００Ａ 
            {
              if ( AllisNum( str ) )
              {
                gtoken.name = str;
                gtoken_vec.push_back( gtoken );
                str = "";
              } // if
            } // if
            
            str = str.append( line, j, 1 );
          } // else if
          else // 非法符號 
          {
            Process( str, line, j );
          } // else
        } // else(string外,正常情況)
      } // for ( line )
      
      line = "";
    } // if ( code[i] == '\n' )
  } // for ( code )
} // Make_token()

void Token_classify() {
  for ( int i = 0 ; i < gtoken_vec.size() ; i++ )
  {
    if ( AllisNum( gtoken_vec[i].name ) || AllisFloat( gtoken_vec[i].name ) ||
         AllisString( gtoken_vec[i].name ) || AllisChar( gtoken_vec[i].name ) ||
         gtoken_vec[i].name == "true" || gtoken_vec[i].name == "false" )
    {
      gtoken_vec[i].category = "Constant";
    } // if
    else if ( gtoken_vec[i].name == "int" ) { gtoken_vec[i].category = "INT"; } // else if
    else if ( gtoken_vec[i].name == "float" ) { gtoken_vec[i].category = "FLOAT"; } // else if
    else if ( gtoken_vec[i].name == "char" ) { gtoken_vec[i].category = "CHAR"; } // else if
    else if ( gtoken_vec[i].name == "bool" ) { gtoken_vec[i].category = "BOOL"; } // else if
    else if ( gtoken_vec[i].name == "string" ) { gtoken_vec[i].category = "STRING"; } // else if
    else if ( gtoken_vec[i].name == "void" ) { gtoken_vec[i].category = "VOID"; } // else if
    else if ( gtoken_vec[i].name == "if" ) { gtoken_vec[i].category = "IF"; } // else if
    else if ( gtoken_vec[i].name == "else" ) { gtoken_vec[i].category = "ELSE"; } // else if
    else if ( gtoken_vec[i].name == "while" ) { gtoken_vec[i].category = "WHILE"; } // else if
    else if ( gtoken_vec[i].name == "do" ) { gtoken_vec[i].category = "DO"; } // else if
    else if ( gtoken_vec[i].name == "return" ) { gtoken_vec[i].category = "RETURN"; } // else if
    else if ( gtoken_vec[i].name == "cout" ) { gtoken_vec[i].category = "COUT"; } // else if
    else if ( gtoken_vec[i].name == "cin" ) { gtoken_vec[i].category = "CIN"; } // else if
    else if ( isalpha( gtoken_vec[i].name[0] ) )
    {
      gtoken_vec[i].category = "ID";
    } // else if
    else if ( gtoken_vec[i].name == "(" || gtoken_vec[i].name == ")" ||
              gtoken_vec[i].name == "[" || gtoken_vec[i].name == "]" ||
              gtoken_vec[i].name == "{" || gtoken_vec[i].name == "}" ||
              gtoken_vec[i].name == "+" || gtoken_vec[i].name == "-" ||
              gtoken_vec[i].name == "*" || gtoken_vec[i].name == "/" ||
              gtoken_vec[i].name == "%" || gtoken_vec[i].name == "^" ||
              gtoken_vec[i].name == "<" || gtoken_vec[i].name == ">" ||
              gtoken_vec[i].name == "&" || gtoken_vec[i].name == "|" ||
              gtoken_vec[i].name == "=" || gtoken_vec[i].name == "!" ||
              gtoken_vec[i].name == "," || gtoken_vec[i].name == ";" ||
              gtoken_vec[i].name == "?" || gtoken_vec[i].name == ":" ||
              gtoken_vec[i].name[0] == '\n' )
    {
      gtoken_vec[i].category = gtoken_vec[i].name;
    } // else if
    else if ( gtoken_vec[i].name == ">=" ) { gtoken_vec[i].category = "GE"; } // else if
    else if ( gtoken_vec[i].name == "<=" ) { gtoken_vec[i].category = "LE"; } // else if
    else if ( gtoken_vec[i].name == "==" ) { gtoken_vec[i].category = "EQ"; } // else if
    else if ( gtoken_vec[i].name == "!=" ) { gtoken_vec[i].category = "NEQ"; } // else if
    else if ( gtoken_vec[i].name == "&&" ) { gtoken_vec[i].category = "AND"; } // else if
    else if ( gtoken_vec[i].name == "||" ) { gtoken_vec[i].category = "OR"; } // else if
    else if ( gtoken_vec[i].name == "+=" ) { gtoken_vec[i].category = "PE"; } // else if
    else if ( gtoken_vec[i].name == "-=" ) { gtoken_vec[i].category = "ME"; } // else if
    else if ( gtoken_vec[i].name == "*=" ) { gtoken_vec[i].category = "TE"; } // else if
    else if ( gtoken_vec[i].name == "/=" ) { gtoken_vec[i].category = "DE"; } // else if
    else if ( gtoken_vec[i].name == "%=" ) { gtoken_vec[i].category = "RE"; } // else if
    else if ( gtoken_vec[i].name == "++" ) { gtoken_vec[i].category = "PP"; } // else if
    else if ( gtoken_vec[i].name == "--" ) { gtoken_vec[i].category = "MM"; } // else if
    else if ( gtoken_vec[i].name == ">>" ) { gtoken_vec[i].category = "RS"; } // else if
    else if ( gtoken_vec[i].name == "<<" ) { gtoken_vec[i].category = "LS"; } // else if
    else
    {
      gtoken_vec[i].category = "illegal";
    } // else
  } // for (int i=0; i<gtoken_vec.size(); i++)
} // Token_classify()

string LastToken( int cur ) { // just pick
  int cur_last = cur - 1; // 為了檢查上一個是不是換行 
  while ( gtoken_vec[cur_last].name[0] == '\n' )
  {
    cur--;
    cur_last = cur - 1;
  } // while
  
  return gtoken_vec[--cur].name;
} // LastToken()

string GetToken( int &cur ) {  // get next gtoken
  int cur_next = cur + 1;
  
  while ( gtoken_vec[cur_next].category[0] == '\n' )
  {
    gLine++;
    cur++;
    cur_next = cur + 1;
  } // while

  if ( ginFuncDef )
  {
    gFuncDef.content.push_back( gtoken_vec[cur_next].name );
  } // if

  // cout << gtoken_vec[cur_next].name; 
  return gtoken_vec[++cur].name;
} // GetToken()

string PeekToken( int cur ) { // see next gtoken
  int cur_next = cur + 1;
  int tmp_line = gLine;
  
  while ( gtoken_vec[cur_next].category[0] == '\n' )
  {
    tmp_line++;
    cur++;
    cur_next = cur + 1;
  } // while
  
  if ( gtoken_vec[cur_next].category == "illegal" ) // error1
  {
    gError1 = true;
    cur++;
    string str = "Line " + Int2str( tmp_line ) +
                " : unrecognized token with first char : '" + gtoken_vec[cur].name + "'";
    throw str.c_str();
  } // if
  
  return gtoken_vec[cur_next].category;
} // PeekToken()

void Output_name() {
  for ( int i = 0 ; i < gtoken_vec.size() ; i++ )
  {
    cout << gtoken_vec[i].name;
    if ( gtoken_vec[i].name[0] != '\n' )
    {
      cout << " ";
    } // if
  } // for
  
  cout << endl;
} // Output_name()

void Output_type() {
  for ( int i = 0 ; i < gtoken_vec.size() ; i++ )
  {
    cout << gtoken_vec[i].category;
    if ( gtoken_vec[i].name[0] != '\n' )
    {
      cout << " ";
    } // if
  } // for
  
  cout << endl; 
} // Output_type()

bool Type_specifier( string type ) {
  // INT | CHAR | FLOAT | STRING | BOOL

  // # ifdef TRACE
  // cout << "[*Type_specifier]";
  // # endif
  
  if ( type == "INT" || type == "CHAR" ||
       type == "FLOAT" || type == "STRING" || type == "BOOL" )
  {
    return true;
  } // if
  else return false;
} // Type_specifier()

void Unsigned_unary_exp( int &cur, string &result, string &type ) {
/*
  Identifier [ '(' [ actual_parameter_list ] ')' 
               |
               [ '[' expression ']' ] [ ( PP | MM ) ]
             ]                                        
*/
  // Constant 
  // '(' expression ')'

  // # ifdef TRACE
  // cout << "[*Unsigned_unary_exp]";
  // # endif

  if ( PeekToken( cur ) == "ID" )
  {
    string id_name = GetToken( cur );
    Check_ID_exist( id_name );
    ID id = FindID( id_name );
    result = id.value;
    
    if ( PeekToken( cur ) == "(" )
    {
      GetToken( cur );
      
      if ( PeekToken( cur ) == ")" )
      {
        GetToken( cur );
      } // if ()
      else
      {
        vector<string> param_vec;
        Actual_parameter_list( cur, param_vec );
        
        if ( PeekToken( cur ) == ")" )
        {
          GetToken( cur );
        } // if
        else { Error2( cur ); } // else
      } // else // '(' Actual_parameter_list ')'
    } // if // [ '(' [ Actual_parameter_list ] ')' ]
    else if ( PeekToken( cur ) == "[" || PeekToken( cur ) == "PP" || PeekToken( cur ) == "MM" )
    {
      gPPMM_isArray = false;
      if ( PeekToken( cur ) == "[" )
      {
        gPPMM_isArray = true;
        GetToken( cur );
        bool inCout = false;
        if ( gCoutState == true )
        {
          inCout = true;
          gCoutState = false;
        } // if
      
        Expression( cur, result, type );
        
        if ( PeekToken( cur ) == "]" )
        {
          GetToken( cur );
        } // if
        else { Error2( cur ); } // else
        
        if ( type != "int" ) { Type_error( "1int", result, type ); } // if index not int
        
        if ( inCout == true ) { gCoutState = true; } // if
        
        int index = atoi( result.c_str() );
        gPPMM_IDindex = index;
        result = id.array[index];
      } // if [ '[' expression ']' ]
      
      if ( PeekToken( cur ) == "PP" || PeekToken( cur ) == "MM" )
      {
        gPPMM_after = true;
        gPPMM_IDname = id_name;
        string ppmm = GetToken( cur );
        
        if ( id.type == "int" || id.type == "float" )
        {
          if ( ppmm == "++" )
          {
            double value = Num( result, id.type ) + 1;
            gPPMM_result = Float2str( value );
          } // if
          else if ( ppmm == "--" )
          {
            double value = Num( result, id.type ) - 1;
            gPPMM_result = Float2str( value );
          } // else if
          
          type = id.type;
        } // if
        else { Type_error( "1int, float", result, type ); } // else
      } // if
    } // else if // [ [ '[' expression ']' ] [ ( PP | MM ) ] ]
    
    type = id.type;
  } // if // ID 那行 
  else if ( PeekToken( cur ) == "Constant" )
  {
    string constant = GetToken( cur );
    result = constant;
    type = Type( result );
  } // else if // Constant
  else
  {
    if ( PeekToken( cur ) == "(" )
    {
      GetToken( cur );
      bool inCout = false;
      if ( gCoutState == true )
      {
        inCout = true;
        gCoutState = false;
      } // if
      
      Expression( cur, result, type );
      
      if ( PeekToken( cur ) == ")" )
      {
        GetToken( cur );
      } // if
      else { Error2( cur ); } // else
      
      if ( inCout == true ) { gCoutState = true; } // if
    } // if
    else { Error2( cur ); } // else
  } // else // '(' Expression ')'
} // Unsigned_unary_exp()

void Signed_unary_exp( int &cur, string &result, string &type, string sign ) {
  // Identifier [ '(' [ Actual_parameter_list ] ')' | '[' Expression ']' ]
  // Constant 
  // '(' Expression ')'

  // # ifdef TRACE
  // cout << "[*Signed_unary_exp]";
  // # endif

  if ( PeekToken( cur ) == "ID" )
  {
    string id_name = GetToken( cur );
    Check_ID_exist( id_name );
    ID id = FindID( id_name );
    result = id.value;
    
    if ( PeekToken( cur ) == "(" )
    {
      GetToken( cur );
      
      if ( PeekToken( cur ) == ")" )
      {
        GetToken( cur );
      } // if ()
      else
      {
        vector<string> param_vec;
        Actual_parameter_list( cur, param_vec );
        
        if ( PeekToken( cur ) == ")" )
        {
          GetToken( cur );
        } // if
        else { Error2( cur ); } // else
      } // else // '(' Actual_parameter_list ')'
    } // if // [ '(' [ Actual_parameter_list ] ')' ]
    else if ( PeekToken( cur ) == "[" )
    {
      GetToken( cur );
      bool inCout = false;
      if ( gCoutState == true )
      {
        inCout = true;
        gCoutState = false;
      } // if
      
      Expression( cur, result, type );
      
      if ( PeekToken( cur ) == "]" )
      {
        GetToken( cur );
      } // if
      else { Error2( cur ); } // else
      
      if ( type != "int" ) { Type_error( "2int", result, type ); } // if
      
      if ( inCout == true ) { gCoutState = true; } // if
      
      int index = atoi( result.c_str() );
      result = id.array[index];
    } // else if // [ '[' Expression ']' ]
    
    type = id.type;
    if ( id.type == "int" || id.type == "float" )
    {
      if ( sign == "-" )
      {
        double value = -Num( result, type );
        result = Float2str( value );
      } // if
    } // if id.type == int | float
    else if ( id.type == "bool" )
    {
      if ( sign == "+!" )
      {
        if ( result == "true" || result == "1" ) { result = "false"; } // if
        else if ( result == "false" || result == "0" ) { result = "true"; } // else if
      } // if
      else if ( sign == "-!" )
      {
        if ( result == "true" || result == "1" ) { result = "-0"; } // if
        else if ( result == "false" || result == "0" ) { result = "-1"; } // else if
      } // else if
    } // else if id.type == bool
    else { Type_error( "int, float, bool", result, type ); } // else
  } // if // ID [ '(' [ Actual_parameter_list ] ')' | '[' Expression ']' ]
  else if ( PeekToken( cur ) == "Constant" )
  {
    string constant = GetToken( cur );
    type = Type( constant );
    if ( type == "int" || type == "float" )
    {
      if ( sign == "-" )
      {
        double value = -Num( constant, type );
        result = Float2str( value );
      } // if
      else
      {
        result = constant;
      } // else
    } // if type == int | float
    else if ( type == "bool" )
    {
      if ( sign == "+!" )
      {
        if ( result == "true" || result == "1" ) { result = "false"; } // if
        else if ( result == "false" || result == "0" ) { result = "true"; } // else if
      } // if
      else if ( sign == "-!" )
      {
        if ( result == "true" || result == "1" ) { result = "-0"; } // if
        else if ( result == "false" || result == "0" ) { result = "-1"; } // else if
      } // else if
    } // else if type == bool
    else { Type_error( "int, float, bool", result, type ); } // else
  } // else if // Constant
  else
  {
    if ( PeekToken( cur ) == "(" )
    {
      GetToken( cur );
      bool inCout = false;
      if ( gCoutState == true )
      {
        inCout = true;
        gCoutState = false;
      } // if
      
      Expression( cur, result, type );
      
      if ( PeekToken( cur ) == ")" )
      {
        GetToken( cur );
      } // if
      else { Error2( cur ); } // else
      
      if ( inCout == true ) { gCoutState = true; } // if
    } // if
    else { Error2( cur ); } // else
    
    if ( type == "int" || type == "float" )
    {
      if ( sign == "-" )
      {
        double value = -Num( result, type );
        result = Float2str( value );
      } // if
    } // if type == int | float
    else if ( type == "bool" )
    {
      if ( sign == "+!" )
      {
        if ( result == "true" || result == "1" ) { result = "false"; } // if
        else if ( result == "false" || result == "0" ) { result = "true"; } // else if
      } // if
      else if ( sign == "-!" )
      {
        if ( result == "true" || result == "1" ) { result = "-0"; } // if
        else if ( result == "false" || result == "0" ) { result = "-1"; } // else if
      } // else if
    } // else if type == bool
    else { Type_error( "int, float, bool", result, type ); } // else
  } // else // '(' Expression ')'
} // Signed_unary_exp()

void Unary_exp( int &cur, string &result, string &type ) {
  // ( PP | MM ) Identifier [ '[' Expression ']' ]
  // Sign { Sign } Signed_unary_exp
  // Unsigned_unary_exp

  // # ifdef TRACE
  // cout << "[*Unary_exp]";
  // # endif
 
  string id_type;
  string id_name;
 
  if ( PeekToken( cur ) == "PP" || PeekToken( cur ) == "MM" )
  {
    string ppmm = GetToken( cur );
    bool isArray = false;
    int index = 0;

    if ( PeekToken( cur ) == "ID" )
    {
      id_name = GetToken( cur );
      Check_ID_exist( id_name );
      ID id = FindID( id_name );
      result = id.value; 
      id_type = id.type;
      
      if ( PeekToken( cur ) == "[" )
      {
        isArray = true;
        GetToken( cur );
        bool inCout = false;
        if ( gCoutState == true )
        {
          inCout = true;
          gCoutState = false;
        } // if
        
        Expression( cur, result, type );
        
        if ( type != "int" ) { Type_error( "3int", result, type ); } // if
        
        if ( PeekToken( cur ) == "]" )
        {
          GetToken( cur );
        } // if
        else { Error2( cur ); } // else
        
        if ( inCout == true ) { gCoutState = true; } // if
        
        index = atoi( result.c_str() );
        result = id.array[index];
      } // if [ '[' Expression ']' ]
    } // if Identifier
    else { Error2( cur ); } // else
    
    type = id_type;
    if ( id_type == "int" || id_type == "float" )
    {
      if ( ppmm == "++" )
      {
        double value = Num( result, type ) + 1;
        result = Float2str( value );
      } // if
      else if ( ppmm == "--" )
      {
        double value = Num( result, type ) - 1;
        result = Float2str( value );
      } // else if
      
      // PPMM_before 先改ＩＤ值 
      if ( isArray == true )
      {
        Assign_IDarray( id_name, index, result );
      } // if
      else
      {
        Assign_ID( id_name, result );
      } // else
    } // if id_type == int | float
    else { Type_error( "2int, float", result, type ); } // else
  } // if // ( PP | MM ) Identifier [ '[' Expression ']' ]
  else if ( Sign( PeekToken( cur ) ) )
  {
    vector<string> sign_vec;
    sign_vec.push_back( GetToken( cur ) );
    while ( Sign( PeekToken( cur ) ) )
    {
      sign_vec.push_back( GetToken( cur ) );
    } // while

    string sign = "+"; 
    for ( int i = 0 ; i < sign_vec.size() ; i++ )
    {
      if ( sign_vec[i] == "-" )
      {
        if ( sign == "+" ) { sign = "-"; } // if
        else if ( sign == "-" ) { sign = "+"; } // else if
      } // if 遇 "-" 變號 
    } // for
    
    for ( int i = 0 ; i < sign_vec.size() ; i++ )
    {
      if ( sign_vec[i] == "!" )
      {
        if ( sign == "+" ) { sign = "+!"; } // if
        else if ( sign == "-" ) { sign = "-!"; } // else if
        else if ( sign == "+!" ) { sign = "+"; } // else if
        else if ( sign == "-!" ) { sign = "-"; } // else if
      } // if 遇 "!" 變號 
    } // for

    Signed_unary_exp( cur, result, type, sign );
  } // else if // Sign { Sign } Signed_unary_exp
  else
  {
    Unsigned_unary_exp( cur, result, type );
  } // else // Unsigned_unary_exp
} // Unary_exp()

void Rest_of_maybe_mult_exp( int &cur, string &result, string &type ) {
  // { ( '*' | '/' | '%' ) Unary_exp }  /* could be empty ! */
  
  // # ifdef TRACE
  // cout << "[*Rest_of_maybe_mult_exp]";
  // # endif
  
  string left_type = type;
  string left_exp = result;
  while ( PeekToken( cur ) == "*" || PeekToken( cur ) == "/" || PeekToken( cur ) == "%" )
  {
    string op = GetToken( cur );
    Unary_exp( cur, result, type );
    string right_type = type;
    string right_exp = result;
    
    if ( op == "*" )
    {
      if ( left_type == "int" && right_type == "int" )
      {
        left_type = "int";
        left_exp = Int2str( atoi( left_exp.c_str() ) * atoi( right_exp.c_str() ) );
      } // if
      else if ( left_type == "float" || right_type == "float" )
      {
        left_type = "float";
        left_exp = Float2str( atof( left_exp.c_str() ) * atof( right_exp.c_str() ) );
      } // else if
      else
      {
        Type_error( "float, int", result, type );
      } // else
    } // if "*"
    else if ( op == "/" )
    {
      if ( left_type == "int" && right_type == "int" )
      {
        left_type = "int";
        if ( right_exp == "0" )
        {
          throw "ERROR: Divide by zero";
        } // if
        else
        {
          left_exp = Int2str( atoi( left_exp.c_str() ) / atoi( right_exp.c_str() ) );
        } // else
      } // if
      else if ( left_type == "float" || right_type == "float" )
      {
        left_type = "float";
        if ( right_exp == "0" )
        {
          throw "ERROR: Divide by zero";
        } // if
        else
        {
          left_exp = Float2str( atof( left_exp.c_str() ) / atof( right_exp.c_str() ) );
        } // else
      } // else if
      else { Type_error( "float, int", result, type ); } // else
    } // else if "/"
    else if ( op == "%" )
    {
      if ( left_type == "int" && right_type == "int" )
      {
        left_type = "int";
        left_exp = Int2str( atoi( left_exp.c_str() ) % atoi( right_exp.c_str() ) );
      } // if
      else { Type_error( "4int", result, type ); } // else
    } // else if "%"

    if ( gPPMM_after == true )
    {
      if ( gPPMM_isArray == true )
      {
        Assign_IDarray( gPPMM_IDname, gPPMM_IDindex, gPPMM_result );
      } // if
      else
      {
        Assign_ID( gPPMM_IDname, gPPMM_result );
      } // else
      
      // gPPMM initialize
      gPPMM_after = false;
      gPPMM_IDname = "";
      gPPMM_isArray = false;
      gPPMM_result = "";
      gPPMM_IDindex = 0;
    } // if gPPMM_after == true
  } // while
  
  type = left_type;
  result = left_exp;
} // Rest_of_maybe_mult_exp()

void Maybe_mult_exp( int &cur, string &result, string &type ) {
  // Unary_exp Rest_of_maybe_mult_exp
  
  // # ifdef TRACE
  // cout << "[*Maybe_mult_exp]";
  // # endif
  
  Unary_exp( cur, result, type );
  Rest_of_maybe_mult_exp( cur, result, type );
  
  if ( gPPMM_after == true )
  {
    if ( gPPMM_isArray == true )
    {
      Assign_IDarray( gPPMM_IDname, gPPMM_IDindex, gPPMM_result );
    } // if
    else
    {
      Assign_ID( gPPMM_IDname, gPPMM_result );
    } // else
    
    // gPPMM initialize
    gPPMM_after = false;
    gPPMM_IDname = "";
    gPPMM_isArray = false;
    gPPMM_result = "";
    gPPMM_IDindex = 0;
  } // if gPPMM_after == true
} // Maybe_mult_exp()

void Rest_of_maybe_additive_exp( int &cur, string &result, string &type ) {
  // Rest_of_maybe_mult_exp { ( '+' | '-' ) Maybe_mult_exp }
  
  // # ifdef TRACE
  // cout << "[*Rest_of_maybe_additive_exp]";
  // # endif
  
  Rest_of_maybe_mult_exp( cur, result, type );
  
  string left_type = type;
  string left_exp = result;
  while ( PeekToken( cur ) == "+" || PeekToken( cur ) == "-" )
  {
    string op = GetToken( cur );
    Maybe_mult_exp( cur, result, type );
    string right_type = type;
    string right_exp = result;
    
    if ( op == "+" )
    {
      if ( left_type == "string" || right_type == "string" )
      {
        left_type = "string";
        left_exp = Get_str_or_char( left_exp ) + Get_str_or_char( right_exp );
      } // if
      else if ( left_type == "float" || right_type == "float" )
      {
        left_type = "float";
        left_exp = Float2str( atof( left_exp.c_str() ) + atof( right_exp.c_str() ) );
      } // else if
      else if ( left_type == "int" && right_type == "int" )
      {
        left_type = "int";
        left_exp = Int2str( atoi( left_exp.c_str() ) + atoi( right_exp.c_str() ) );
      } // else if
      else
      {
        Type_error( "string, float, int", result, type );
      } // else
      
      if ( left_type == "string" )
      {
        left_exp = "\"" + left_exp + "\"";
      } // if
    } // if "+"
    else if ( op == "-" )
    {
      if ( left_type == "int" && right_type == "int" )
      {
        left_type = "int";
        left_exp = Int2str( atoi( left_exp.c_str() ) - atoi( right_exp.c_str() ) );
      } // if
      else if ( left_type == "float" || right_type == "float" )
      {
        left_type = "float";
        left_exp = Float2str( atof( left_exp.c_str() ) - atof( right_exp.c_str() ) );
      } // else if
      else
      {
        Type_error( "float, int", result, type );
      } // else
    } // else if "-"
  } // while { ( '+' | '-' ) Maybe_mult_exp }
  
  type = left_type;
  result = left_exp;
} // Rest_of_maybe_additive_exp()

void Maybe_additive_exp( int &cur, string &result, string &type ) {
  // Maybe_mult_exp { ( '+' | '-' ) Maybe_mult_exp }
  
  // # ifdef TRACE
  // cout << "[*Maybe_additive_exp]";
  // # endif
  
  Maybe_mult_exp( cur, result, type );
  
  string left_type = type;
  string left_exp = result;
  while ( PeekToken( cur ) == "+" || PeekToken( cur ) == "-" )
  {
    string op = GetToken( cur );
    Maybe_mult_exp( cur, result, type );
    string right_type = type;
    string right_exp = result;
    
    if ( op == "+" )
    {
      if ( left_type == "string" || right_type == "string" )
      {
        left_type = "string";
        left_exp = left_exp + right_exp;
      } // if
      else if ( left_type == "float" || right_type == "float" )
      {
        left_type = "float";
        left_exp = Float2str( atof( left_exp.c_str() ) + atof( right_exp.c_str() ) );
      } // else if
      else if ( left_type == "int" && right_type == "int" )
      {
        left_type = "int";
        left_exp = Int2str( atoi( left_exp.c_str() ) + atoi( right_exp.c_str() ) );
      } // else if
      else
      {
        Type_error( "string, float, int", result, type );
      } // else
      
      if ( left_type == "string" )
      {
        left_exp = "\"" + left_exp + "\"";
      } // if
    } // if "+"
    else if ( op == "-" )
    {
      if ( left_type == "float" || right_type == "float" )
      {
        left_type = "float";
        left_exp = Float2str( atof( left_exp.c_str() ) - atof( right_exp.c_str() ) );
      } // if
      else if ( left_type == "int" && right_type == "int" )
      {
        left_type = "int";
        left_exp = Int2str( atoi( left_exp.c_str() ) - atoi( right_exp.c_str() ) );
      } // else if
      else
      {
        Type_error( "float, int", result, type );
      } // else
    } // else if "-"
  } // while { ( '+' | '-' ) Maybe_mult_exp }
  
  type = left_type;
  result = left_exp;
} // Maybe_additive_exp()

void Rest_of_maybe_shift_exp( int &cur, string &result, string &type ) {
  // Rest_of_maybe_additive_exp { ( LS | RS ) Maybe_additive_exp }
  
  // # ifdef TRACE
  // cout << "[*Rest_of_maybe_shift_exp]";
  // # endif
  
  Rest_of_maybe_additive_exp( cur, result, type );
  
  int i = 0;
  int left_int = 0;
  bool haveOP = false;
  while ( gCoutState == false && ( PeekToken( cur ) == "LS" || PeekToken( cur ) == "RS" ) )
  {
    haveOP = true;
    if ( i == 0 )
    {
      if ( type == "int" )
      {
        left_int = atoi( result.c_str() );
      } // if
      else { Type_error( "5int", result, type ); } // else
    } // if ( i == 0 )

    string op = GetToken( cur ); // "<<" | ">>"
    Maybe_additive_exp( cur, result, type );
    int right_int = 0;
    
    if ( type == "int" && result != "0" )
    {
      right_int = atoi( result.c_str() );
    } // if
    else { Type_error( "6int", result, type ); } // else
    
    if ( op == "<<" )
    {
      left_int = left_int << right_int;
    } // if LS
    else if ( op == ">>" )
    {
      left_int = left_int >> right_int;
    } // else if RS
    
    i++;
  } // while { ( LS | RS ) Maybe_additive_exp }
  
  if ( haveOP == true )
  {
    result = Int2str( left_int );
    type = "int";
  } // if
} // Rest_of_maybe_shift_exp()

void Maybe_shift_exp( int &cur, string &result, string &type ) {
  // Maybe_additive_exp { ( LS | RS ) Maybe_additive_exp }
  
  // # ifdef TRACE
  // cout << "[*Maybe_shift_exp]";
  // # endif
  
  Maybe_additive_exp( cur, result, type );
  
  int i = 0;
  int left_int = 0;
  bool haveOP = false;
  while ( PeekToken( cur ) == "LS" || PeekToken( cur ) == "RS" )
  {
    haveOP = true;
    if ( i == 0 )
    {
      if ( type == "int" )
      {
        left_int = atoi( result.c_str() );
      } // if
      else { Type_error( "7int", result, type ); } // else
    } // if ( i == 0 )

    string op = GetToken( cur ); // "<<" | ">>"
    Maybe_additive_exp( cur, result, type );

    int right_int = 0;
    if ( type == "int" && result != "0" )
    {
      right_int = atoi( result.c_str() );
    } // if
    else { Type_error( "8int", result, type ); } // else
    
    if ( op == "<<" )
    {
      left_int = left_int << right_int;
    } // if LS
    else if ( op == ">>" )
    {
      left_int = left_int >> right_int;
    } // else if RS
    
    i++;
  } // while { ( LS | RS ) Maybe_additive_exp }
  
  if ( haveOP == true )
  {
    result = Int2str( left_int );
    type = "int";
  } // if
} // Maybe_shift_exp()

void Rest_of_maybe_relational_exp( int &cur, string &result, string &type ) {
  // Rest_of_maybe_shift_exp { ( '<' | '>' | LE | GE ) Maybe_shift_exp }

  // # ifdef TRACE
  // cout << "[*Rest_of_maybe_relational_exp]";
  // # endif

  Rest_of_maybe_shift_exp( cur, result, type );
  
  bool haveOP = false;
  
  double left_num = 0;
  if ( PeekToken( cur ) == "<" || PeekToken( cur ) == ">" ||
       PeekToken( cur ) == "LE" || PeekToken( cur ) == "GE" )
  {
    left_num = Num( result, type );
  } // if

  while ( PeekToken( cur ) == "<" || PeekToken( cur ) == ">" ||
          PeekToken( cur ) == "LE" || PeekToken( cur ) == "GE" )
  {
    haveOP = true;
    string op = GetToken( cur );
    Maybe_shift_exp( cur, result, type );
    double right_num = Num( result, type );
    
    if ( op == "<" )
    {
      if ( left_num < right_num ) { left_num = 1; } // if
      else { left_num = 0; } // else
    } // if <
    else if ( op == ">" )
    {
      if ( left_num > right_num ) { left_num = 1; } // if
      else { left_num = 0; } // else
    } // else if >
    else if ( op == "<=" )
    {
      if ( left_num <= right_num ) { left_num = 1; } // if
      else { left_num = 0; } // else
    } // else if LE
    else if ( op == ">=" )
    {
      if ( left_num >= right_num ) { left_num = 1; } // if
      else { left_num = 0; } // else
    } // else if GE
  } // while { ( '<' | '>' | LE | GE ) Maybe_shift_exp }
  
  if ( haveOP == true )
  {
    result = Float2str( left_num );
    type = "bool";
  } // if
} // Rest_of_maybe_relational_exp()

void Maybe_relational_exp( int &cur, string &result, string &type ) {
  // Maybe_shift_exp { ( '<' | '>' | LE | GE ) Maybe_shift_exp }

  // # ifdef TRACE
  // cout << "[*Maybe_relational_exp]";
  // # endif

  Maybe_shift_exp( cur, result, type );
  
  bool haveOP = false;

  double left_num = 0;
  if ( PeekToken( cur ) == "<" || PeekToken( cur ) == ">" ||
       PeekToken( cur ) == "LE" || PeekToken( cur ) == "GE" )
  {
    left_num = Num( result, type );
  } // if

  while ( PeekToken( cur ) == "<" || PeekToken( cur ) == ">" ||
          PeekToken( cur ) == "LE" || PeekToken( cur ) == "GE" )
  {
    haveOP = true;
    string op = GetToken( cur );
    Maybe_shift_exp( cur, result, type );
    double right_num = Num( result, type );
    
    if ( op == "<" )
    {
      if ( left_num < right_num ) { left_num = 1; } // if
      else { left_num = 0; } // else
    } // if <
    else if ( op == ">" )
    {
      if ( left_num > right_num ) { left_num = 1; } // if
      else { left_num = 0; } // else
    } // else if >
    else if ( op == "<=" )
    {
      if ( left_num <= right_num ) { left_num = 1; } // if
      else { left_num = 0; } // else
    } // else if LE
    else if ( op == ">=" )
    {
      if ( left_num >= right_num ) { left_num = 1; } // if
      else { left_num = 0; } // else
    } // else if GE
  } // while { ( '<' | '>' | LE | GE ) Maybe_shift_exp }
  
  if ( haveOP == true )
  {
    result = Float2str( left_num );
    type = "bool";
  } // if
} // Maybe_relational_exp()

void Rest_of_maybe_equality_exp( int &cur, string &result, string &type ) {
  // Rest_of_maybe_relational_exp { ( EQ | NEQ ) Maybe_relational_exp }

  // # ifdef TRACE
  // cout << "[*Rest_of_maybe_equality_exp]";
  // # endif

  Rest_of_maybe_relational_exp( cur, result, type );
  
  string left_exp = result;
  bool haveOP = false;
  while ( PeekToken( cur ) == "EQ" || PeekToken( cur ) == "NEQ" )
  {
    haveOP = true;
    string op = GetToken( cur );
    Maybe_relational_exp( cur, result, type );
    string right_exp = result;
    
    if ( op == "==" )
    {
      if ( left_exp == right_exp )
      {
        left_exp = "true";  
      } // if
      else if ( ( left_exp == "1" || left_exp == "true" ) &&
                ( right_exp == "1" || right_exp == "true" ) )
      {
        left_exp = "true";
      } // else if
      else if ( ( left_exp == "0" || left_exp == "false" ) &&
                ( right_exp == "0" || right_exp == "false" ) )
      {
        left_exp = "true";
      } // else if
      else
      {
        left_exp = "false";
      } // else
    } // if EQ
    else if ( op == "!=" )
    {
      if ( left_exp == right_exp )
      {
        left_exp = "false";  
      } // if
      else if ( ( left_exp == "1" || left_exp == "true" ) &&
                ( right_exp == "1" || right_exp == "true" ) )
      {
        left_exp = "false";
      } // if
      else if ( ( left_exp == "0" || left_exp == "false" ) &&
                ( right_exp == "0" || right_exp == "false" ) )
      {
        left_exp = "false";
      } // else if
      else
      {
        left_exp = "true";
      } // else
    } // else if NEQ
  } // while { ( EQ | NEQ ) Maybe_relational_exp }
  
  if ( haveOP == true )
  {
    result = left_exp;
    type = "bool";
  } // if
} // Rest_of_maybe_equality_exp()

void Maybe_equality_exp( int &cur, string &result, string &type ) {
  // Maybe_relational_exp { ( EQ | NEQ ) Maybe_relational_exp }

  // # ifdef TRACE
  // cout << "[*Maybe_equality_exp]";
  // # endif

  Maybe_relational_exp( cur, result, type );
  
  string left_exp = result;
  bool haveOP = false;
  while ( PeekToken( cur ) == "EQ" || PeekToken( cur ) == "NEQ" )
  {
    haveOP = true;
    string op = GetToken( cur );
    Maybe_relational_exp( cur, result, type );
    string right_exp = result;
    
    if ( op == "==" )
    {
      if ( left_exp == right_exp )
      {
        left_exp = "true";  
      } // if
      else if ( ( left_exp == "1" || left_exp == "true" ) &&
                ( right_exp == "1" || right_exp == "true" ) )
      {
        left_exp = "true";
      } // else if
      else if ( ( left_exp == "0" || left_exp == "false" ) &&
                ( right_exp == "0" || right_exp == "false" ) )
      {
        left_exp = "true";
      } // else if
      else
      {
        left_exp = "false";
      } // else
    } // if EQ
    
    if ( op == "!=" )
    {
      if ( left_exp == right_exp )
      {
        left_exp = "false";  
      } // if
      else if ( ( left_exp == "1" || left_exp == "true" ) &&
                ( right_exp == "1" || right_exp == "true" ) )
      {
        left_exp = "false";
      } // if
      else if ( ( left_exp == "0" || left_exp == "false" ) &&
                ( right_exp == "0" || right_exp == "false" ) )
      {
        left_exp = "false";
      } // else if
      else
      {
        left_exp = "true";
      } // else
    } // if NEQ
  } // while { ( EQ | NEQ ) Maybe_relational_exp }
  
  if ( haveOP == true )
  {
    result = left_exp;
    type = "bool";
  } // if
} // Maybe_equality_exp()

void Rest_of_maybe_bit_AND_exp( int &cur, string &result, string &type ) {
  // Rest_of_maybe_equality_exp { '&' Maybe_equality_exp }
  
  // # ifdef TRACE
  // cout << "[*Rest_of_maybe_bit_AND_exp]";
  // # endif
  
  Rest_of_maybe_equality_exp( cur, result, type );
  
  while ( PeekToken( cur ) == "&" )
  {
    GetToken( cur );
    Maybe_equality_exp( cur, result, type );
  } // while { '&' Maybe_equality_exp }
} // Rest_of_maybe_bit_AND_exp()

void Maybe_bit_AND_exp( int &cur, string &result, string &type ) {
  // Maybe_equality_exp { '&' Maybe_equality_exp }
  
  // # ifdef TRACE
  // cout << "[*Maybe_bit_AND_exp]";
  // # endif
  
  Maybe_equality_exp( cur, result, type );
  
  while ( PeekToken( cur ) == "&" )
  {
    GetToken( cur );
    Maybe_equality_exp( cur, result, type );
  } // while { '&' Maybe_equality_exp }
} // Maybe_bit_AND_exp()

void Rest_of_maybe_bit_ex_OR_exp( int &cur, string &result, string &type ) {
  // Rest_of_maybe_bit_AND_exp { '^' Maybe_bit_AND_exp }
  
  // # ifdef TRACE
  // cout << "[*Rest_of_maybe_bit_ex_OR_exp]";
  // # endif
  
  Rest_of_maybe_bit_AND_exp( cur, result, type );
  
  while ( PeekToken( cur ) == "^" )
  {
    GetToken( cur );
    Maybe_bit_AND_exp( cur, result, type );
  } // while { '^' Maybe_bit_AND_exp }
} // Rest_of_maybe_bit_ex_OR_exp()

void Maybe_bit_ex_OR_exp( int &cur, string &result, string &type ) {
  // Maybe_bit_AND_exp { '^' Maybe_bit_AND_exp }
  
  // # ifdef TRACE
  // cout << "[*Maybe_bit_ex_OR_exp]";
  // # endif
  
  Maybe_bit_AND_exp( cur, result, type );
  
  while ( PeekToken( cur ) == "^" )
  {
    GetToken( cur );
    Maybe_bit_AND_exp( cur, result, type );
  } // while { '^' Maybe_bit_AND_exp }
} // Maybe_bit_ex_OR_exp()

void Rest_of_maybe_bit_OR_exp( int &cur, string &result, string &type ) {
  // Rest_of_maybe_bit_ex_OR_exp { '|' Maybe_bit_ex_OR_exp }
  
  // # ifdef TRACE
  // cout << "[*Rest_of_maybe_bit_OR_exp]";
  // # endif
  
  Rest_of_maybe_bit_ex_OR_exp( cur, result, type );
  
  while ( PeekToken( cur ) == "|" )
  {
    GetToken( cur );
    Maybe_bit_ex_OR_exp( cur, result, type );
  } // while { '|' Maybe_bit_ex_OR_exp }
} // Rest_of_maybe_bit_OR_exp()

void Maybe_bit_OR_exp( int &cur, string &result, string &type ) {
  // Maybe_bit_ex_OR_exp { '|' Maybe_bit_ex_OR_exp }
  
  // # ifdef TRACE
  // cout << "[*Maybe_bit_OR_exp]";
  // # endif
  
  Maybe_bit_ex_OR_exp( cur, result, type );
  
  while ( PeekToken( cur ) == "|" )
  {
    GetToken( cur );
    Maybe_bit_ex_OR_exp( cur, result, type );
  } // while { '|' Maybe_bit_ex_OR_exp }
} // Maybe_bit_OR_exp()

void Rest_of_maybe_logical_AND_exp( int &cur, string &result, string &type ) {
  // Rest_of_maybe_bit_OR_exp { AND Maybe_bit_OR_exp }
  
  // # ifdef TRACE
  // cout << "[*Rest_of_maybe_logical_AND_exp]";
  // # endif
  
  Rest_of_maybe_bit_OR_exp( cur, result, type );
  
  vector<string> and_vec;
  int i = 0; // and_count
  while ( PeekToken( cur ) == "AND" )
  {
    if ( i == 0 )
    {
      and_vec.push_back( result );
    } // if
    
    GetToken( cur );
    Maybe_bit_OR_exp( cur, result, type );
    and_vec.push_back( result );
    
    i++;
  } // while { AND Maybe_bit_OR_exp }
  
  if ( !and_vec.empty() )
  {
    result = "true";

    for ( int i = 0 ; i < and_vec.size() ; i++ )
    {
      if ( and_vec[i] == "false" || and_vec[i] == "0" )
      {
        result = "false";
      } // if
    } // for
    
    type = "bool";
  } // if have AND
} // Rest_of_maybe_logical_AND_exp()

void Maybe_logical_AND_exp( int &cur, string &result, string &type ) {
  // Maybe_bit_OR_exp { AND Maybe_bit_OR_exp }
  
  // # ifdef TRACE
  // cout << "[*Maybe_logical_AND_exp]";
  // # endif
  
  Maybe_bit_OR_exp( cur, result, type );
  
  vector<string> and_vec;
  int i = 0; // and_count
  while ( PeekToken( cur ) == "AND" )
  {
    if ( i == 0 )
    {
      and_vec.push_back( result );
    } // if
    
    GetToken( cur );
    Maybe_bit_OR_exp( cur, result, type );
    and_vec.push_back( result );
    
    i++;
  } // while { AND Maybe_bit_OR_exp }
  
  if ( !and_vec.empty() )
  {
    result = "true";

    for ( int i = 0 ; i < and_vec.size() ; i++ )
    {
      if ( and_vec[i] == "false" || and_vec[i] == "0" )
      {
        result = "false";
      } // if
    } // for
    
    type = "bool";
  } // if have AND
} // Maybe_logical_AND_exp()

void Rest_of_maybe_logical_OR_exp( int &cur, string &result, string &type ) {
  // Rest_of_maybe_logical_AND_exp { OR maybe_logical_AND_exp }
  
  // # ifdef TRACE
  // cout << "[*Rest_of_maybe_logical_OR_exp]";
  // # endif
  
  Rest_of_maybe_logical_AND_exp( cur, result, type );
  
  vector<string> or_vec;
  int i = 0; // or_count
  while ( PeekToken( cur ) == "OR" )
  {
    if ( i == 0 )
    {
      or_vec.push_back( result );
    } // if

    GetToken( cur ); // "or"
    Maybe_logical_AND_exp( cur, result, type );
    or_vec.push_back( result );
    
    i++;
  } // while { OR Maybe_logical_AND_exp }
  
  if ( !or_vec.empty() )
  {
    result = "false";

    for ( int i = 0 ; i < or_vec.size() ; i++ )
    {
      if ( or_vec[i] == "true" || or_vec[i] == "1" )
      {
        result = "true";
      } // if
    } // for
    
    type = "bool";
  } // if have OR
} // Rest_of_maybe_logical_OR_exp()

void Romce_and_romloe( int &cur, string &result, string &type ) {
  // Rest_of_maybe_logical_OR_exp [ '?' basic_expression ':' basic_expression ]
  
  // # ifdef TRACE
  // cout << "[*Romce_and_romloe]";
  // # endif
  
  Rest_of_maybe_logical_OR_exp( cur, result, type );
  
  if ( PeekToken( cur ) == "?" )
  {
    GetToken( cur );
    gCondTRUE = true;
    string cond_result = result;
    gCondResult_vec.push_back( cond_result );
    
    if ( !Cond1_Assignable() ) { gCondTRUE = false; } // if
  
    string exp1 = Basic_expression( cur, result, type );
    gCondTRUE = true;
    
    if ( PeekToken( cur ) == ":" )
    {
      GetToken( cur );
      
      if ( !Cond2_Assignable() ) { gCondTRUE = false; } // if

      string exp2 = Basic_expression( cur, result, type );
      gCondTRUE = true;

      if ( cond_result == "true" || cond_result == "1" )
      {
        result = exp1;
      } // if
      else if ( cond_result == "false" || cond_result == "0" )
      {
        result = exp2;
      } // else if
      else
      {
        Type_error( "2bool", cond_result, type );
      } // else
    } // if
    else { Error2( cur ); } // else
    
    gCondResult_vec.erase( gCondResult_vec.end() - 1 );
  } // if [ '?' Basic_expression ':' Basic_expression ]
  
  if ( type == "bool" && result == "1" )
  {
    result = "true";
  } // if
  else if ( type == "bool" && result == "0" )
  {
    result = "false";
  } // else if
} // Romce_and_romloe()

bool Assignment_operator( string op ) {
  // '=' | TE | DE | RE | PE | ME
  
  // # ifdef TRACE
  // cout << "[*Assignment_operator]";
  // # endif
  
  if ( op == "=" || op == "TE" ||
       op == "DE" || op == "RE" ||
       op == "PE" || op == "ME" )
  {
    return true;
  } // if
  else return false;
} // Assignment_operator()

void Actual_parameter_list( int &cur, vector<string> &param_vec ) {
  // basic_expression { ',' basic_expression }
  
  // # ifdef TRACE
  // cout << "[*Actual_parameter_list]";
  // # endif
  
  string result, type;

  param_vec.push_back( Basic_expression( cur, result, type ) );
  while ( PeekToken( cur ) == "," )
  {
    GetToken( cur );
    param_vec.push_back( Basic_expression( cur, result, type ) );
  } // while
} // Actual_parameter_list()

bool Sign( string sign ) {
  // '+' | '-' | '!'
  
  // # ifdef TRACE
  // cout << "[*Sign]";
  // # endif
  
  if ( sign == "+" || sign == "-" || sign == "!" )
  {
    return true;
  } // if
  else return false;
} // Sign()

void Rest_of_PPMM_ISBE( int &cur, string ppmm, string id_name, string &result, string &type ) {
  // [ '[' expression ']' ] Romce_and_romloe
  
  // # ifdef TRACE
  // cout << "[*Rest_of_PPMM_ISBE]";
  // # endif
  
  result = FindID( id_name ).value;
  type = Type( result );
  
  bool isArray = false;
  int index;
  if ( PeekToken( cur ) == "[" )
  {
    isArray = true;
    GetToken( cur );
    bool inCout = false;
    if ( gCoutState == true )
    {
      inCout = true;
      gCoutState = false;
    } // if
    
    Expression( cur, result, type );
    if ( type != "int" ) { Type_error( "9int", result, type ); } // if
    
    if ( PeekToken( cur ) == "]" )
    {
      GetToken( cur );
    } // if
    else { Error2( cur ); } // else
    
    if ( inCout == true ) { gCoutState = true; } // if
    
    index = atoi( result.c_str() );
    result = FindID( id_name ).array[index];
    type = Type( result );
  } // if [ '[' expression ']' ]
  
  if ( type == "int" || type == "float" )
  {
    if ( ppmm == "++" )
    {
      double value = Num( result, type ) + 1;
      result = Float2str( value );
    } // if PP
    else if ( ppmm == "--" )
    {
      double value = Num( result, type ) - 1;
      result = Float2str( value );
    } // else if MM
    
    // PPMM_before 先改ＩＤ值 
    if ( isArray == true )
    {
      Assign_IDarray( id_name, index, result );
    } // if
    else
    {
      Assign_ID( id_name, result );
    } // else
  } // if type == int | float
  else { Type_error( "0int, float", result, type ); } // else
  
  Romce_and_romloe( cur, result, type );
} // Rest_of_PPMM_ISBE()

void Rest_of_ISBE( int &cur, string id_name, string &result, string &type ) {
  // '(' [ actual_parameter_list ] ')' romce_and_romloe
/*
  [ '[' expression ']' ]
  ( assignment_operator basic_expression | [ PP | MM ] romce_and_romloe )
*/
  
  // # ifdef TRACE
  // cout << "[*Rest_of_ISBE]";
  // # endif

  if ( PeekToken( cur ) == "(" )
  {
    GetToken( cur );
    vector<string> param_vec;

    if ( PeekToken( cur ) == ")" )
    {
      GetToken( cur );
    } // if ()
    else
    {
      Actual_parameter_list( cur, param_vec );
      if ( PeekToken( cur ) == ")" )
      {
        GetToken( cur );
      } // if
      else { Error2( cur ); } // else
    } // else '(' Actual_parameter_list ')'

    // function在這邊做事
    Do_function( id_name, param_vec, result );
    Romce_and_romloe( cur, result, type );
  } // if // '(' [ Actual_parameter_list ] ')' Romce_and_romloe
  else
  {
    bool isArray = false;
    int index = 0;
    if ( PeekToken( cur ) == "[" )
    {
      isArray = true;
      GetToken( cur );
      bool inCout = false;
      if ( gCoutState == true )
      {
        inCout = true;
        gCoutState = false;
      } // if
      
      Expression( cur, result, type );
      if ( type == "int" )
      {
        index = atoi( result.c_str() );
      } // if
      else { Type_error( "10int", result, type ); } // else
      
      if ( PeekToken( cur ) == "]" )
      {
        GetToken( cur );
      } // if
      else { Error2( cur ); } // else
      
      if ( inCout == true ) { gCoutState = true; } // if
    } // if [ '[' expression ']' ]
    
    if ( Assignment_operator( PeekToken( cur ) ) )
    {
      string op = GetToken( cur ); // "=" | "*=" | "/=" | "%=" | "+=" | "-="
      Basic_expression( cur, result, type );
      
      if ( op == "=" )
      {
        if ( isArray == true )
        {
          Assign_IDarray( id_name, index, result );
        } // if isArray
        else
        {
          Assign_ID( id_name, result );
        } // else isArray == false
      } // if "="
      else if ( op == "*=" )
      {
        if ( type == "int" || type == "float" )
        {
          if ( isArray == true )
          {
            double value = Num( FindID( id_name ).array[index], type ) * atof( result.c_str() );
            result = Float2str( value );
            Assign_IDarray( id_name, index, result );
          } // if
          else
          {
            double value = Num( FindID( id_name ).value, type ) * atof( result.c_str() );
            result = Float2str( value );
            Assign_ID( id_name, result );
          } // else
        } // if
        else { Type_error( "3int, float", result, type ); } // else
      } // else if "*="
      else if ( op == "/=" )
      {
        if ( result == "0" )
        {
          if ( type == "int" || type == "float" )
          {
            if ( isArray == true )
            {
              double value = Num( FindID( id_name ).array[index], type ) / atof( result.c_str() );
              result = Float2str( value );
              Assign_IDarray( id_name, index, result );
            } // if
            else
            {
              double value = Num( FindID( id_name ).value, type ) / atof( result.c_str() );
              result = Float2str( value );
              Assign_ID( id_name, result );
            } // else
          } // if
          else { Type_error( "4int, float", result, type ); } // else
        } // if
        else { throw "ERROR: Divide by zero"; } // else
      } // else if "/="
      else if ( op == "%=" )
      {
        if ( type == "int" )
        {
          if ( isArray == true )
          {
            int value = atoi( FindID( id_name ).array[index].c_str() ) % atoi( result.c_str() );
            result = Int2str( value );
            Assign_IDarray( id_name, index, result );
          } // if isArray == true
          else
          {
            int value = atoi( FindID( id_name ).value.c_str() ) % atoi( result.c_str() );
            result = Int2str( value );
            Assign_ID( id_name, result );
          } // else
        } // if
        else { Type_error( "11int", result, type ); } // else 
      } // else if "%="
      else if ( op == "+=" )
      {
        if ( type == "int" || type == "float" || type == "string" )
        {
          if ( isArray == true )
          {
            if ( type == "string" )
            {
              string str = Get_str_or_char( FindID( id_name ).array[index] ) + Get_str_or_char( result );
              str = "\"" + str + "\"";
              result = str;
              Assign_IDarray( id_name, index, result );
            } // if type == "string"
            else
            {
              double value = Num( FindID( id_name ).array[index], type ) + atof( result.c_str() );
              result = Float2str( value );
              Assign_IDarray( id_name, index, result );
            } // else type == int | float
          } // if isArray
          else
          {
            if ( type == "string" )
            {
              string str = Get_str_or_char( FindID( id_name ).value ) + Get_str_or_char( result );
              result = str;
              str = "\"" + str + "\"";
              Assign_ID( id_name, result );
            } // if type == "string"
            else
            {
              double value = Num( FindID( id_name ).value, type ) + atof( result.c_str() );
              result = Float2str( value );
              Assign_ID( id_name, result );
            } // else type == int | float
          } // else isArray == false
        } // if
        else { Type_error( "int, float, string", result, type ); } // else
      } // else if "+="
      else if ( op == "-=" )
      {
        if ( type == "int" || type == "float" )
        {
          if ( isArray == true )
          {
            double value = Num( FindID( id_name ).array[index], type ) - atof( result.c_str() );
            result = Float2str( value );
            Assign_IDarray( id_name, index, result );
          } // if
          else
          {
            double value = Num( FindID( id_name ).value, type ) - atof( result.c_str() );
            result = Float2str( value );
            Assign_ID( id_name, result );
          } // else
        } // if
        else { Type_error( "5int, float", result, type ); } // else
      } // else if "-="
    } // if Assignment_operator Basic_expression
    else
    {
      if ( isArray == true )
      {
        result = FindID( id_name ).array[index];
      } // if
      else
      {
        result = FindID( id_name ).value;
      } // else
      
      type = Type( result );
      
      bool havePPMM = false;
      double ppmm_result;
      if ( PeekToken( cur ) == "PP" || PeekToken( cur ) == "MM" )
      {
        havePPMM = true;
        string ppmm = GetToken( cur );
        
        if ( type == "int" || type == "float" )
        {
          if ( ppmm == "++" )
          {
            ppmm_result = Num( result, type ) + 1;
          } // if "++"
          else if ( ppmm == "--" )
          {
            ppmm_result = Num( result, type ) - 1;
          } // else if "--"
        } // if
        else { Type_error( "6int, float", result, type ); } // else
      } // if [ PP | MM ]
       
      Romce_and_romloe( cur, result, type );
      
      // PPMM_after 運算完再改ＩＤ值
      if ( havePPMM == true )
      {
        if ( isArray == true )
        {
          Assign_IDarray( id_name, index, Float2str( ppmm_result ) );
        } // if
        else
        {
          Assign_ID( id_name, Float2str( ppmm_result ) );
        } // else
      } // if  havePPMM && PPMM_after 運算完再改ＩＤ值
    } // else [ PP | MM ] Romce_and_romloe
  } // else // [ '[' expression ']' ] ( Assignment_operator Basic_expression | [ PP | MM ] romce_and_romloe )
} // Rest_of_ISBE()

string Basic_expression( int &cur, string &result, string &type ) {
  // Identifier rest_of_Identifier_started_basic_exp
  // ( PP | MM ) Identifier rest_of_PPMM_Identifier_started_basic_exp
  // ( Constant | '(' expression ')' ) romce_and_romloe
  // sign { sign } signed_unary_exp romce_and_romloe

  // # ifdef TRACE
  // cout << "[*Basic_expression]";
  // # endif

  if ( PeekToken( cur ) == "ID" )
  {
    string id_name = GetToken( cur );
    Check_ID_exist( id_name );
    Rest_of_ISBE( cur, id_name, result, type );
  } // if // Identifier Rest_of_ISBE
  else if ( PeekToken( cur ) == "PP" || PeekToken( cur ) == "MM" )
  {
    string ppmm = GetToken( cur );
    
    if ( PeekToken( cur ) == "ID" )
    {
      string id_name = GetToken( cur );
      Check_ID_exist( id_name );
      Rest_of_PPMM_ISBE( cur, ppmm, id_name, result, type );
    } // if
    else { Error2( cur ); } // else
  } // else if // ( PP | MM ) Identifier Rest_of_PPMM_ISBE
  else if ( PeekToken( cur ) == "Constant" || PeekToken( cur ) == "(" )
  {
    if ( PeekToken( cur ) == "(" )
    {
      GetToken( cur );
      bool inCout = false;
      if ( gCoutState == true )
      {
        inCout = true;
        gCoutState = false;
      } // if

      Expression( cur, result, type );
      
      if ( PeekToken( cur ) == ")" )
      {
        GetToken( cur );
      } // if
      else { Error2( cur ); } // else
      
      if ( inCout == true ) { gCoutState = true; } // if
    } // if '(' expression ')'
    else if ( PeekToken( cur ) == "Constant" )
    {
      result = GetToken( cur );
      type = Type( result );
    } // else if Constant
    
    Romce_and_romloe( cur, result, type );
  } // else if // ( Constant | '(' expression ')' ) romce_and_romloe
  else
  {
    vector<string> sign_vec;
    do
    {
      if ( Sign( PeekToken( cur ) ) )
      {
        sign_vec.push_back( GetToken( cur ) );
      } // if
      else { Error2( cur ); } // else
    } while ( Sign( PeekToken( cur ) ) );
    
    string sign = "+"; // 先不考慮 "!" 
    for ( int i = 0 ; i < sign_vec.size() ; i++ )
    {
      if ( sign_vec[i] == "-" )
      {
        if ( sign == "+" ) { sign = "-"; } // if
        else if ( sign == "-" ) { sign = "+"; } // else if
      } // if 遇 "-" 變號 
    } // for
    
    for ( int i = 0 ; i < sign_vec.size() ; i++ )
    {
      if ( sign_vec[i] == "!" )
      {
        if ( sign == "+" ) { sign = "+!"; } // if
        else if ( sign == "-" ) { sign = "-!"; } // else if
        else if ( sign == "+!" ) { sign = "+"; } // else if
        else if ( sign == "-!" ) { sign = "-"; } // else if
      } // if 遇 "!" 變號 
    } // for

    Signed_unary_exp( cur, result, type, sign );
    Romce_and_romloe( cur, result, type );
  } // else // Sign { Sign } Signed_unary_exp Romce_and_romloe
  
  return result;
} // Basic_expression()

vector<string> Expression( int &cur, string &result, string &type ) {
  // Basic_expression { ',' Basic_expression }
  // cout { LS Expression } ';'
  
  // # ifdef TRACE
  // cout << "[*Expression]";
  // # endif

  vector<string> out_vec;
  if ( PeekToken( cur ) == "COUT" )
  {
    GetToken( cur );
    
    if ( PeekToken( cur ) != "LS" ) { Error2( cur ); } // if ( cout後一定要是<< )
 
    while ( PeekToken( cur ) == "LS" )
    {
      gCoutState = true;
      GetToken( cur );
      Expression( cur, result, type );
      
      if ( gCondTRUE )
      {
        out_vec.push_back( result );
      } // if
    } // while
    
    gCoutState = false;
  } // if cout <<
  else if ( PeekToken( cur ) == "CIN" )
  {
    GetToken( cur );
    
    if ( PeekToken( cur ) != "RS" ) { Error2( cur ); } // if ( cout後一定要是<< )
 
    while ( PeekToken( cur ) == "RS" )
    {
      GetToken( cur );
      Expression( cur, result, type );
    } // while
  } // else if cin >>
  else
  {
    // 目前沒處理多個basic_exp, 只會回傳最後做的result 
    Basic_expression( cur, result, type );
    while ( PeekToken( cur ) == "," )
    {
      GetToken( cur );
      Basic_expression( cur, result, type );
    } // while
  } // else
  
  return out_vec;
} // Expression()

void Statement( int &cur ) {
  // ';'     // the null statement
  // Expression ';'  /* expression here should not be empty */
  // RETURN [ expression ] ';'
  // Compound_statement
  // IF '(' expression ')' statement [ ELSE statement ]
  // WHILE '(' expression ')' statement
  // DO statement WHILE '(' expression ')' ';'
  
  // # ifdef TRACE
  // cout << "[*Statement]";
  // # endif
  
  string result = "";
  string type = "";
  
  if ( PeekToken( cur ) == ";" )
  {
    GetToken( cur );
  } // if
  else if ( PeekToken( cur ) == "RETURN" )
  {
    GetToken( cur );
    
    if ( PeekToken( cur ) == ";" )
    {
      GetToken( cur );
    } // if
    else
    {
      Expression( cur, result, type );
      if ( PeekToken( cur ) == ";" )
      {
        GetToken( cur );
      } // if
      else { Error2( cur ); } // else
    } // else
  } // else if RETURN [ expression ] ';'
  else if ( PeekToken( cur ) == "IF" )
  {
    GetToken( cur );
    
    gCondTRUE = true;
    string if_result = result;
    if ( PeekToken( cur ) == "(" )
    {
      GetToken( cur );
      Expression( cur, result, type );
      if_result = result;
      
      if ( PeekToken( cur ) == ")" )
      {
        GetToken( cur );
        
        if ( if_result == "true" || if_result == "1" )
        {
          gAssignable_vec.push_back( true );
        } // if
        else
        {
          gAssignable_vec.push_back( false );
        } // else
        
        if ( !Assignable() ) { gCondTRUE = false;  } // if
        
        Statement( cur );
        
        gAssignable_vec.erase( gAssignable_vec.end() - 1 );
        if ( Assignable() ) { gCondTRUE = true;  } // if
      } // if
      else { Error2( cur ); } // else
    } // if
    else { Error2( cur ); } // else

    if ( PeekToken( cur ) == "ELSE" )
    {
      GetToken( cur );
      
      if ( if_result == "false" || if_result == "0" )
      {
        gAssignable_vec.push_back( true );
      } // if
      else
      {
        gAssignable_vec.push_back( false );
      } // else
      
      if ( !Assignable() ) { gCondTRUE = false;  } // if
      
      Statement( cur );
      
      gAssignable_vec.erase( gAssignable_vec.end() - 1 );
      if ( Assignable() ) { gCondTRUE = true;  } // if
    } // if ELSE
  } // else if IF '(' expression ')' statement [ ELSE statement ]
  else if ( PeekToken( cur ) == "WHILE" )
  {
    GetToken( cur );
    
    if ( PeekToken( cur ) == "(" )
    {
      GetToken( cur );
      
      // 紀錄條件式的初始值 
      int cond_cur = cur;
      string cond_result = result; // 好像不用 
      string cond_type = type; // 好像不用 
      Expression( cur, result, type );
      string while_result = result;
      gCondTRUE = true;
      
      if ( PeekToken( cur ) == ")" )
      {
        GetToken( cur );
        
        if ( while_result == "true" || while_result == "1" )
        {
          gAssignable_vec.push_back( true );
        } // if
        else if ( while_result == "false" || while_result == "0" )
        {
          gAssignable_vec.push_back( false );
        } // else if
        
        if ( !Assignable() ) { gCondTRUE = false;  } // if
        
        if ( while_result == "false" || while_result == "0" )
        {
          Statement( cur );
        } // if
        
        // else
        int state_cur = cur;
        int state_cur_end = cur;
        while ( while_result == "true" || while_result == "1" )
        {
          int tmp_state_cur = state_cur;
          Statement( tmp_state_cur );
          state_cur_end = tmp_state_cur;
          
          int tmp_cur = cond_cur;
          string tmp_result = cond_result; // 好像不用  
          string tmp_type = cond_type; // 好像不用  
          Expression( tmp_cur, tmp_result, tmp_type ); // 條件式 
          while_result = tmp_result;
        } // while
        
        cur = state_cur_end;

        gAssignable_vec.erase( gAssignable_vec.end() - 1 );
        if ( Assignable() ) { gCondTRUE = true;  } // if
      } // if ')'
      else { Error2( cur ); } // else
    } // if
    else { Error2( cur ); } // else
  } // else if WHILE '(' expression ')' statement
  else if ( PeekToken( cur ) == "DO" )
  {
    GetToken( cur );
    
    int state_cur = cur; // 紀錄重複執行的位置 
    Statement( cur );

    int cond_cur; // 紀錄重複執行條件式的位置 
    string cond_result; // 好像不用 
    string cond_type; // 好像不用 
    string while_result = "false";
    if ( PeekToken( cur ) == "WHILE" )
    {
      GetToken( cur );
      
      if ( PeekToken( cur ) == "(" )
      {
        GetToken( cur );
        
        cond_cur = cur;
        cond_result = result;
        cond_type = type;
        Expression( cur, result, type );
        
        if ( PeekToken( cur ) == ")" ) 
        {
          GetToken( cur );
          
          if ( PeekToken( cur ) == ";" )
          {
            GetToken( cur );
          } // if
          else { Error2( cur ); } // else
        } // if
        else { Error2( cur ); } // else
      } // if
      else { Error2( cur ); } // else
    } // if
    else { Error2( cur ); } // else
    
    while_result = result;
    while ( while_result == "true" || while_result == "1" )
    {
      int tmp_state_cur = state_cur;
      Statement( tmp_state_cur );
      
      int tmp_cur = cond_cur;
      string tmp_result = cond_result; // 好像不用  
      string tmp_type = cond_type; // 好像不用  
      Expression( tmp_cur, tmp_result, tmp_type ); // 條件式 
      while_result = tmp_result;
    } // while repeat
  } // else if DO statement WHILE '(' expression ')' ';'
  else if ( PeekToken( cur ) == "{" )
  {
    Compound_statement( cur );
  } // else if Compound_statement
  else
  {
    vector<string> out_vec;
    if ( PeekToken( cur ) == "COUT" )
    {
      out_vec = Expression( cur, result, type );
    } // if
    else
    {
      Expression( cur, result, type );
    } // else
    
    if ( PeekToken( cur ) == ";" )
    {
      GetToken( cur );
      
      if ( !out_vec.empty() )
      {
        for ( int i = 0 ; i < out_vec.size() ; i++ )
        {
          if ( AllisString( out_vec[i] ) || AllisChar( out_vec[i] ) )
          {
            string str = Get_str_or_char( out_vec[i] );
            for ( int i = 0 ; i < str.size() ; i++ )
            {
              if ( str[i] == '\\' )
              {
                if ( i < str.size() - 1 && str[i+1] == 'n' )
                {
                  cout << endl;
                  i++;
                } // if
              } // if '\n'
              else
              {
                cout << str[i];
              } // else
            } // for
          } // if out_vec[i] == string | char
          else
          {
            cout << out_vec[i];
          } // else
        } // for out_vec.size()
      } // if
    } // if
    else { Error2( cur ); } // else
  } // else Expression ';'  /* expression here should not be empty */
} // Statement()

void Declaration( int &cur ) {
  // type_specifier Identifier Rest_of_declarators
  
  // # ifdef TRACE
  // cout << "[*Declaration]";
  // # endif
  
  string type = GetToken( cur );
  
  if ( PeekToken( cur ) == "ID" )
  {
    string id = GetToken( cur );
    
    Rest_of_declarators( cur, type, id );
  } // if
  else
  {
    Error2( cur );
  } // else
} // Declaration()

void Compound_statement( int &cur ) {
  // '{' { declaration | statement } '}'
  
  // # ifdef TRACE
  // cout << "[*Compound_statement]";
  // # endif
  
  if ( PeekToken( cur ) == "{" )
  {
    ginLocal = true;
    gCompoundLayer++;
    GetToken( cur );

    while ( PeekToken( cur ) != "}" )
    {
      if ( Type_specifier( PeekToken( cur ) ) ) // Declaration
      {
        Declaration( cur );
      } // if
      else
      {
        Statement( cur );
      } // else
    } // while { declaration | statement }
    
    if ( PeekToken( cur ) == "}" )
    {
      Local_table_pop();
      
      gCompoundLayer--;
      if ( gCompoundLayer == 0 )
      {
        ginFuncDef = false;
        ginLocal = false;
      } // if

      GetToken( cur );
    } // if {}
  } // if
  else { Error2( cur ); } // else
} // Compound_statement()

void Formal_parameter_list( int &cur, vector<Param> &params ) {
/*
  Type_specifier [ '&' ] Identifier [ '[' Constant ']' ] 
  { ',' Type_specifier [ '&' ] Identifier [ '[' Constant ']' ] }
*/

  // # ifdef TRACE
  // cout << "[*Formal_parameter_list]";
  // # endif

  int i = 1; // 計算現在是宣告第幾個ID 
  do
  {
    Param param;

    if ( i > 1 )
    {
      if ( PeekToken( cur ) == "," )
      {
        GetToken( cur );
      } // if
      else { Error2( cur );  } // else
    } // if 宣告第二個之後的ID
    
    if ( Type_specifier( PeekToken( cur ) ) )
    {
      param.type = GetToken( cur );

      if ( PeekToken( cur ) == "&" )
      {
        param.ref = GetToken( cur );
      } // if
      
      if ( PeekToken( cur ) == "ID" )
      {
        param.name = GetToken( cur );
        
        if ( PeekToken( cur ) == "[" )
        {
          GetToken( cur );

          if ( PeekToken( cur ) == "Constant" ) // size
          {
            param.size = GetToken( cur );
            
            if ( PeekToken( cur ) == "]" )
            {
              GetToken( cur );
            } // if
            else { Error2( cur ); } // else
          } // if
        } // if [ '[' Constant ']' ]
      } // if
      else { Error2( cur ); } // else
    } // if Type_specifier
    else { Error2( cur ); } // else
    
    params.push_back( param );
    i++;
  } while ( PeekToken( cur ) == "," );
} // Formal_parameter_list()

void Function_definition_without_ID( int &cur, string func_type, string func_id ) {
  // '(' [ VOID | formal_parameter_list ] ')' Compound_statement
  
  // # ifdef TRACE
  // cout << "[*Function_definition_without_ID]";
  // # endif

  gFuncDef.name = func_id;
  gFuncDef.type = func_type;
  
  if ( PeekToken( cur ) == "(" )
  {
    GetToken( cur );
    
    if ( PeekToken( cur ) == ")" )
    {
      GetToken( cur );
    } // if ()
    else if ( PeekToken( cur ) == "VOID" )
    {
      GetToken( cur );
      
      if ( PeekToken( cur ) == ")" )
      {
        GetToken( cur );
      } // if
      else { Error2( cur ); } // else
    } // else if ( VOID )
    else
    {
      vector<Param> params;
      Formal_parameter_list( cur, params );
      gFuncDef.formal_param_vec = params;
      
      if ( PeekToken( cur ) == ")" )
      {
        GetToken( cur );
      } // if
      else { Error2( cur ); } // else
    } // else ( formal_parameter_list )
    
    ginFuncDef = true;
    Compound_statement( cur );
    Make_func_table();
  } // if (
  else { Error2( cur ); } // else
} // Function_definition_without_ID()

void Rest_of_declarators( int &cur, string type, string id ) {
  // [ '[' Constant ']' ] { ',' Identifier [ '[' Constant ']' ] } ';'

  // # ifdef TRACE
  // cout << "[*Rest_of_declarators]";
  // # endif

  vector<ID> tempid_vec;
  gid.type = type;
  gid.name = id;
  gid.size = -1;
  gid.value = "";
  gid.layer = gCompoundLayer;

  if ( PeekToken( cur ) == "[" )
  {
    GetToken( cur );
    if ( PeekToken( cur ) == "Constant" )
    {
      gid.size = atoi( GetToken( cur ).c_str() );
      gid.array.resize( gid.size );
      
      if ( PeekToken( cur ) == "]" )
      {
        GetToken( cur );
      } // if
      else { Error2( cur ); } // else
    } // if
    else { Error2( cur ); } // else
  } // if [ '[' Constant ']' ]

  tempid_vec.push_back( gid );
  
  while ( PeekToken( cur ) == "," )
  {
    GetToken( cur );
    
    if ( PeekToken( cur ) == "ID" )
    {
      gid.type = type;
      gid.name = GetToken( cur );
      gid.size = -1;
      gid.value = "";
      gid.layer = gCompoundLayer;

      if ( PeekToken( cur ) == "[" )
      {
        GetToken( cur );
        if ( PeekToken( cur ) == "Constant" )
        {
          gid.size = atoi( GetToken( cur ).c_str() );
          gid.array.resize( gid.size );

          if ( PeekToken( cur ) == "]" )
          {
            GetToken( cur );
          } // if
          else { Error2( cur ); } // else
        } // if
        else { Error2( cur ); } // else
      } // if [ '[' Constant ']' ]

      tempid_vec.push_back( gid );
    } // if 
    else { Error2( cur ); } // else
  } // while { ',' Identifier [ '[' Constant ']' ] }
  
  if ( PeekToken( cur ) == ";" )
  {
    GetToken( cur );
    if ( ginFuncDef )
    {
      Make_Func_locIDtable( tempid_vec );
    } // if
    else if ( ginLocal )
    {
      Make_locID_table( tempid_vec );
    } // else if local variable
    else
    {
      Make_ID_table( tempid_vec );
    } // else global variable
  } // if
  else { Error2( cur ); } // else
} // Rest_of_declarators()

void Function_definition_or_declarators( int &cur, string type, string id ) {
  // Function_definition_without_ID
  // Rest_of_declarators
  
  // # ifdef TRACE
  // cout << "[*Function_definition_or_declarators]";
  // # endif
  
  if ( PeekToken( cur ) == "(" )
  {
    Function_definition_without_ID( cur, type, id );
  } // if Function_definition_without_ID
  else
  {
    Rest_of_declarators( cur, type, id );
  } // else Rest_of_declarators
} // Function_definition_or_declarators()

void Definition( int &cur ) {
  //           VOID  ID  function_definition_without_ID
  // type_specifier  ID  function_definition_or_declarators

  // # ifdef TRACE
  // cout << "[*Definition]";
  // # endif

  if ( PeekToken( cur ) == "VOID" )
  {
    string func_type = GetToken( cur ); // "void"(小寫) 
    
    if ( PeekToken( cur ) == "ID" )
    {
      string func_id = GetToken( cur );

      Function_definition_without_ID( cur, func_type, func_id );
    } // if
    else
    {
      Error2( cur );
    } // else
  } // if (VOID)
  else
  {
    string type = GetToken( cur );
    
    if ( PeekToken( cur ) == "ID" )
    {
      string id = GetToken( cur );

      Function_definition_or_declarators( cur, type, id );
    } // if
    else
    {
      Error2( cur );
    } // else
  } // else (Type_specifier)
} // Definition()

void User_input( int &cur ) {
  // definition | statement

  // # ifdef TRACE
  // cout << "[*User_input]";
  // # endif

  if ( PeekToken( cur ) == "VOID" || Type_specifier( PeekToken( cur ) ) )
  {
    Definition( cur );
  } // if (definition)
  else
  {
    Statement( cur );
    if ( gIsQuit == false )
    {
      cout << "Statement executed ...";
    } // if
    else
    {
      cout << "Our-C exited ...";
    } // else
  } // else (statement)
} // User_input()

void Grammar() {
  int cur = -1; // current_token
  
  do
  {
    try {
      if ( cur == -1 && PeekToken( cur ) == "Constant" )
      {
        GetToken( cur );
        cout << "Our-C running ..." << endl;
      } // if

      gLine = 0; // every cmd has '\n' in the head (nonono

      cout << "> ";
      User_input( cur );
      cout << endl;

      Init_gFuncDef();
      ginFuncDef = false;
    } // try
    catch ( char const* str ) {
      cout << str << endl;

      if ( gError1 == true )
      {
        GetToken( cur );
        gError1 = false;
      } // if

      while ( gtoken_vec[++cur].category[0] != '\n' ) { ; } // while - pass until next line
      
      cur--; // 回到\n前一個token 

      Init_gFuncDef();
      ginFuncDef = false;
      gCompoundLayer = 0;
    } // catch
  } while ( gIsQuit == false && cur+1 <= gtoken_vec.size() );
} // Grammar()

int main() {
  char c;
  string code;

  while ( !cin.eof() ) // ctrl+z 模擬 EOF 
  {
    cin.get( c );
    code += c;
  } // while
  // cout<<code<<endl;
  
  // Scanner
  Make_token( code );
  Token_classify();
  // Output_name();
  // Output_type();

  Grammar();
  
  // system("pause");
  return 0;
} // main()
