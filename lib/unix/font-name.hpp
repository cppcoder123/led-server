//
//
//
#ifndef UNIX_FONT_NAME
#define UNIX_FONT_NAME

#include <string>
#include <vector>

namespace unix
{
  class font_name_t
  {

  public:
    
    enum id_t {
      id_zero = 0,
      id_greek = id_zero,
      id_ibm,
      id_slim,
      id_size                   // keep last
    };

    static const std::string& get (id_t id);
    static id_t get (const std::string &name);

  private:

    typedef std::vector<std::string> vector_t;
    static vector_t init ();
  };

  //
  //
  //
  inline const std::string& font_name_t::get (id_t id)
  {
    static const vector_t vector (init ());
    static const std::string empty;
    
    if (id >= id_size)
      return empty;
    
    return vector[id];
  }

  inline font_name_t::id_t font_name_t::get (const std::string &name)
  {
    // do we need a map here?
    static const vector_t vector (init ());

    for (std::size_t index = id_zero; index < id_size; ++index) {
      id_t id (static_cast<id_t>(index));
      if (vector[id] == name)
        return id;
    }

    return id_ibm;
  }
  
  inline font_name_t::vector_t font_name_t::init ()
  {
    vector_t result (id_size, "");

    result [id_greek] = "greek";
    result [id_ibm] = "ibm";
    result [id_slim] = "slim";

    return result;
  }
  
} // namespace unix

#endif
