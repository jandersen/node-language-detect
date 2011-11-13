#include <v8.h>
#include <node.h>
#include <string>
#include <vector>

#include "encodings/compact_lang_det/compact_lang_det.h"
#include "encodings/compact_lang_det/ext_lang_enc.h"
#include "base/string_util.h"
#include "cld_encodings.h"

using namespace node;
using namespace v8;

#define REQ_FUN_ARG(I, VAR)                                             \
  if (args.Length() <= (I) || !args[I]->IsFunction())                   \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be a function")));  \
  Local<Function> VAR = Local<Function>::Cast(args[I]);

#define REQ_STRING_ARG(I, VAR)                                             \
  if (args.Length() <= (I) || !args[I]->IsString())                   \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be a string")));  \
  Local<String> VAR = Local<String>::Cast(args[I]);


class LanguageDetector : ObjectWrap {
public:
    static Persistent<FunctionTemplate> s_ct;
    static void Init(Handle<Object> target)
     {
        HandleScope scope;

        Local<FunctionTemplate> t = FunctionTemplate::New(New);

        s_ct = Persistent<FunctionTemplate>::New(t);
        s_ct->InstanceTemplate()->SetInternalFieldCount(1);
        s_ct->SetClassName(String::NewSymbol("LanguageDetector"));

        NODE_SET_PROTOTYPE_METHOD(s_ct, "detect", Detect);
        
        target->Set(String::NewSymbol("LanguageDetector"),
                        s_ct->GetFunction());
      }
      
      LanguageDetector() {
          
      }
      
      ~LanguageDetector() {
          
      }
      
      static Handle<Value> New(const Arguments& args)
      {
          HandleScope scope;
          LanguageDetector* hw = new LanguageDetector();
          hw->Wrap(args.This());
          return args.This();
      }
      
      struct language_record_t {
          std::string name;
          std::string code;
          float percent;
          float normalized_score;
      };
      
      struct language_baton_t {
            LanguageDetector *hw;
            v8::String::Utf8Value *str;
            Persistent<Function> cb;
            std::vector<language_record_t> languages;
          };
      
      
      static Handle<Value> Detect(const Arguments& args)
        {
          HandleScope scope;
          
          v8::String::Utf8Value *v8str = new v8::String::Utf8Value(args[0]);
          
          LanguageDetector* hw = ObjectWrap::Unwrap<LanguageDetector>(args.This());
          language_baton_t *baton = new language_baton_t();
          baton->str = v8str;
          
            char *bytes = **baton->str;
            int numBytes = baton->str->length();

              int isPlainText = 0;
              int pickSummaryLanguage = 0;
              int removeWeakMatches = 1;
              int includeExtendedLanguages = 1;

              Language hintLanguageEnum = UNKNOWN_LANGUAGE;
              Encoding hintEncodingEnum = UNKNOWN_ENCODING;
              char * hintTopLevelDomain = NULL;
              bool isReliable;
              Language language3[3];
              int percent3[3];
              double normalized_score3[3];
              int textBytesFound;
              CompactLangDet::DetectLanguage(0,
                                            bytes, numBytes,
                                            isPlainText != 0,
                                            includeExtendedLanguages != 0,
                                            pickSummaryLanguage != 0,
                                            removeWeakMatches != 0,
                                                       hintTopLevelDomain,
                                                       hintEncodingEnum,
                                                       hintLanguageEnum,
                                                       language3,
                                                       percent3,
                                                       normalized_score3,
                                                       &textBytesFound,
                                                       &isReliable);
            
              for(int idx=0;idx<3;idx++) {
                Language lang = language3[idx];
                if (lang == UNKNOWN_LANGUAGE) {
                  break;
                }
                language_record_t newlang;
                newlang.name = ExtLanguageName(lang);
                newlang.code = ExtLanguageCode(lang);
                newlang.percent = percent3[idx];
                newlang.normalized_score = normalized_score3[idx];
                baton->languages.push_back(newlang);
              }

            Local<Value> argv[1];
            Local<Array> array = Array::New(baton->languages.size());
            int count = 0;
            for(std::vector<language_record_t>::iterator iter = baton->languages.begin(); iter != baton->languages.end(); iter++) {
                language_record_t lang = *iter;
                Local<Object> langDesc = Object::New();
                langDesc->Set(String::New("name"), String::New(lang.name.c_str()));
                langDesc->Set(String::New("code"), String::New(lang.code.c_str()));
                langDesc->Set(String::New("percentile"), Number::New(lang.percent));
                langDesc->Set(String::New("normalizedScore"), Number::New(lang.normalized_score));
                array->Set(Integer::New(count++), langDesc);
            }
            delete baton;
            return array;
          }
    };

Persistent<FunctionTemplate> LanguageDetector::s_ct;

extern "C" void init (Handle<Object> target)
  {
    LanguageDetector::Init(target);
  }

extern "C"  NODE_MODULE(languageDetect, init);
