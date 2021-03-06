#include <iostream>
#include <boost/bind.hpp>
#include <pangolin/pangolin.h>

#include <pangolin/simple_math.h>

using namespace pangolin;
using namespace std;

struct CustomType
{
  int x;
  float y;
  string z;
};

std::ostream& operator<< (std::ostream& os, const CustomType& o){
  os << o.x << " " << o.y << " " << o.z;
  return os;
}

std::istream& operator>> (std::istream& is, CustomType& o){
  is >> o.x;
  is >> o.y;
  is >> o.z;
  return is;
}

void GlobalKeyHook(const std::string& example)
{
    cout << example << endl;
}

int main( int /*argc*/, char* argv[] )
{  
  // Load configuration data
  pangolin::ParseVarsFile("app.cfg");

  // Create OpenGL window in single line thanks to GLUT
  pangolin::CreateGlutWindowAndBind("Main",640,480);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Issue specific OpenGl we might need
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Define Camera Render Object (for view / scene browsing)
  pangolin::OpenGlRenderState s_cam(
    ProjectionMatrix(640,480,420,420,320,240,0.1,1000),
    ModelViewLookAt(-0,0.5,-3, 0,0,0, AxisY)
  );

  const int UI_WIDTH = 180;

  // Add named OpenGL viewport to window and provide 3D Handler
  View& d_cam = pangolin::CreateDisplay()
    .SetBounds(0.0, 1.0, Attach::Pix(UI_WIDTH), 1.0, -640.0f/480.0f)
    .SetHandler(new Handler3D(s_cam));

  // Add named Panel and bind to variables beginning 'ui'
  // A Panel is just a View with a default layout and input handling
  View& d_panel = pangolin::CreatePanel("ui")
      .SetBounds(0.0, 1.0, 0.0, Attach::Pix(UI_WIDTH));

  // Demonstration of how we can register a keyboard hook to alter a Var
  pangolin::RegisterKeyPressCallback( PANGO_CTRL + 'b', SetVarFunctor<double>("ui.A Double", 3.5) );

  // Demonstration of how we can register a keyboard hook to trigger a method
  pangolin::RegisterKeyPressCallback( PANGO_CTRL + 'r', boost::bind(GlobalKeyHook, "You Pushed ctrl-r!" ) );

  // Default hooks for exiting (Esc) and fullscreen (tab).
  while( !pangolin::ShouldQuit() )
  {
    if(pangolin::HasResized())
      DisplayBase().ActivateScissorAndClear();

    // Safe and efficient binding of named variables.
    // Specialisations mean no conversions take place for exact types
    // and conversions between scalar types are cheap.
    static Var<bool> a_button("ui.A Button",false,false);
    static Var<double> a_double("ui.A Double",3,0,5);
    static Var<int> an_int("ui.An Int",2,0,5);
    static Var<double> a_double_log("ui.Log scale var",3,1,1E4, true);
    static Var<bool> a_checkbox("ui.A Checkbox",false,true);
    static Var<int> an_int_no_input("ui.An Int No Input",2);
    static Var<CustomType> any_type("ui.Some Type",(CustomType){0,1.2,"Hello"});

#ifdef HAVE_PNG
    static Var<bool> save_window("ui.Save Window",false,false);
    static Var<bool> save_teapot("ui.Save Teapot",false,false);
#endif // HAVE_PNG

    if( Pushed(a_button) )
      cout << "You Pushed a button!" << endl;

    // Overloading of Var<T> operators allows us to treat them like
    // their wrapped types, eg:
    if( a_checkbox )
      an_int = a_double;

    if( !any_type->z.compare("robot"))
        any_type = (CustomType){1,2.3,"Boogie"};

    an_int_no_input = an_int;

#ifdef HAVE_PNG
    if( Pushed(save_window) )
        pangolin::SaveWindowOnRender("window");

    if( Pushed(save_teapot) )
        d_cam.SaveOnRender("teapot");
#endif // HAVE_PNG

    // Activate efficiently by object
    // (3D Handler requires depth testing to be enabled)
    d_cam.ActivateScissorAndClear(s_cam);

    glEnable(GL_DEPTH_TEST);
    glColor3f(1.0,1.0,1.0);

    // Render some stuff
    glutWireTeapot(1.0);

    // Render our UI panel when we receive input
    if(HadInput())
      d_panel.Render();

    // Swap frames and Process Events
    pangolin::FinishGlutFrame();
  }

  return 0;
}
