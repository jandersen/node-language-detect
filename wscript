srcdir = "."
blddir = "build"
VERSION = "1"


def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")
  
def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon") 
  obj.cxxflags = ["-g", "-D_FILE_OFFSET_BITS=64", "-D_LARGEFILE_SOURCE","-DCLD_WINDOWS"]
  # This is the name of our extension.
  obj.includes = "src"
  obj.target = "language_detect"
  obj.source = """src/encodings/compact_lang_det/cldutil.cc \
          src/encodings/compact_lang_det/cldutil_dbg_empty.cc \
          src/encodings/compact_lang_det/compact_lang_det.cc \
          src/encodings/compact_lang_det/compact_lang_det_impl.cc \
          src/encodings/compact_lang_det/ext_lang_enc.cc \
          src/encodings/compact_lang_det/getonescriptspan.cc \
          src/encodings/compact_lang_det/letterscript_enum.cc \
          src/encodings/compact_lang_det/tote.cc \
          src/encodings/compact_lang_det/generated/cld_generated_score_quadchrome_0406.cc \
          src/encodings/compact_lang_det/generated/compact_lang_det_generated_cjkbis_0.cc \
          src/encodings/compact_lang_det/generated/compact_lang_det_generated_ctjkvz.cc \
          src/encodings/compact_lang_det/generated/compact_lang_det_generated_deltaoctachrome.cc \
          src/encodings/compact_lang_det/generated/compact_lang_det_generated_quadschrome.cc \
          src/encodings/compact_lang_det/win/cld_htmlutils_windows.cc \
          src/encodings/compact_lang_det/win/cld_unilib_windows.cc \
          src/encodings/compact_lang_det/win/cld_utf8statetable.cc \
          src/encodings/compact_lang_det/win/cld_utf8utils_windows.cc \
          src/encodings/internal/encodings.cc \
          src/languages/internal/languages.cc \
          src/language_detect.cc"""
