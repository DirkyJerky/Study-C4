#define demo_int(X) printf( #X " -> %d\n", (X) )
const char *___ret;
int ___retb;
#define demo_string(X) ___ret = (X); printf( #X " -> \"%s\"\n", ((___ret == NULL) ? "NULL" : ___ret) )
#define demo_float(X) printf( #X " -> %f\n", (X) )
#define demo_void(X) printf( #X "\n" ); X
#define demo_char(X) printf( #X " -> \"%c\"\n", (X) )
#define demo_pointer(X) printf( #X " -> \"%p\"\n", (X) )
#define demo_bool(X) ___retb = (X); printf( #X " -> %s\n", ((___retb) ? "true" : "false" ))

#define demo(X) demo_int(X)

#define CONSIG(X) gtk_builder_add_callback_symbol(builder , #X , G_CALLBACK(X))

void gtk_builder_add_callback_symbol(GtkBuilder*, const char*, GCallback);
