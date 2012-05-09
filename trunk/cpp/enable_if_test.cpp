#include <cstdio>
#include <string>

struct true_type {  
    enum { value = 1 }; 
};

struct false_type { 
    enum { value = 0 }; 
};

template <typename T, typename U>
struct is_same : false_type {};

template <typename T>
struct is_same<T, T> : true_type {};

template <typename T> struct is_char_pointer              : false_type {};
template <> struct is_char_pointer<char *>                : true_type  {};
template <> struct is_char_pointer<const char *>          : true_type  {};
template <> struct is_char_pointer<volatile char *>       : true_type  {};
template <> struct is_char_pointer<const volatile char *> : true_type  {};

template <unsigned N> struct is_char_pointer<char[N]>                 : true_type  {};
template <unsigned N> struct is_char_pointer<const char[N]>           : true_type  {};
template <unsigned N> struct is_char_pointer<volatile char[N]>        : true_type  {};
template <unsigned N> struct is_char_pointer<volatile const char[N]>  : true_type  {};

template <typename T, typename U>
struct are_both_char_pointers
{ 
	enum { value = is_char_pointer<T>::value && 
	               is_char_pointer<U>::value };
};

template <typename T, typename U>
struct are_both_same_and_not_char_pointers
{
    enum { value = is_same<T, U>::value       &&
                   !is_char_pointer<T>::value && 
                   !is_char_pointer<U>::value     };
};

template <typename T> struct is_string              : false_type {};
template <> struct is_string<std::string>           : true_type  {};
template <> struct is_string<std::string &>         : true_type  {};
template <> struct is_string<const std::string>     : true_type  {};
template <> struct is_string<const std::string &>   : true_type  {};

template<typename T, typename U>
struct are_both_strings
{
    enum { value = is_char_pointer<T>::value &&
                   is_string<U>::value            };
};

template <bool, typename T = void>
struct enable_if { };

template <typename T>
struct enable_if<true, T> 
{ 
    typedef T type;
};

template <bool, typename T = void>
struct disable_if 
{ 
    typedef T type;
};

template <typename T>
struct disable_if<true, T> { };

typedef int RequestHandle;

template <typename TReq, typename TRep>
class Replier
{
public:

	template <typename URep>
    void send_reply(const URep & rep, 
                    const RequestHandle & handle,
                    typename disable_if<is_char_pointer<URep>::value>::type * = 0);
	
    // VC++ does not like a boolean expression in enable_if.
    // That's why the boolean expression is refactored in are_both_char_pointers.
    template <typename URep>
    void send_reply(URep rep, 
                    const RequestHandle & handle,
                    typename enable_if<are_both_char_pointers<TRep, URep>::value>::type * = 0);

};

template <typename TReq, typename TRep>
template <typename URep>
void Replier<TReq, TRep>::send_reply(const URep & rep, 
                                     const RequestHandle & handle,
                                     typename disable_if<is_char_pointer<URep>::value>::type *)
{
    printf("URep is a NOT pointer.\n");
}

template <typename TReq, typename TRep>
template <typename URep>
void Replier<TReq, TRep>::send_reply(URep rep, 
                                     const RequestHandle & handle,
                                     typename enable_if<are_both_char_pointers<TRep, URep>::value>::type *)
{
    printf("URep is a pointer.\n");
}

template <typename TReq, typename TRep>
class Requester
{
public:

    void send_request(const TReq & req);

    template <typename UReq>
    typename enable_if<are_both_strings<TReq, UReq>::value>::type
    send_request(const UReq & req);

    template <typename UReq>
    typename enable_if<are_both_char_pointers<TReq, UReq>::value>::type
    send_request(WriteSample<UReq> & req);

    template <typename UReq>
    typename enable_if<are_both_same_and_not_char_pointers<TReq, UReq>::value>::type
    send_request(WriteSample<UReq> & req);
};
/*
template <typename TReq, typename TRep>
void Requester<TReq, TRep>::send_request(const TReq & req)
{

}

template <typename TReq, typename TRep>
template <typename UReq>
typename enable_if<are_both_strings<TReq, UReq>::value, void>::type
Requester<TReq, TRep>::send_request(const UReq & req)
{

}

template <typename TReq, typename TRep>
template <typename UReq>
typename enable_if<are_both_char_pointers<TReq, UReq>::value, void>::type
Requester<TReq, TRep>::send_request(WriteSample<UReq> & req)
{

}

template <typename TReq, typename TRep>
template <typename UReq>
typename enable_if<are_both_same_and_not_char_pointers<TReq, UReq>::value, void>::type
Requester<TReq, TRep>::send_request(WriteSample<UReq> & req)
{

}
*/
int main(void)
{
	Replier<char *, char *> r1;
    r1.send_reply("RTI", 999);

	Replier<int, int> r2;
    r2.send_reply(999, 999);
}
/*
foo   foo     OK            OK
foo   char*   LONG ERRORS   LESS ERRORS
char* foo     LONG ERRORS   LONG ERRORS
char* char*   OK            OK
*/
