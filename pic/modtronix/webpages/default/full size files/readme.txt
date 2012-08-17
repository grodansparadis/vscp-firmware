These files are used as source for the *.cgi files found in "default"
folder (one folder down). Seeing that all of these files had code in
them that was often repeated, that code was assigned to tags that
was added to the "Modtronix SBC65EC Demo Web Server". So, in stead of
including the repeated code multiple times, only the tag is included
which is than replaced by the CGI server with the original code.

Each of these files have a couple of commented instruction lines in them
stating what strings have to be replaced with tags. For example:
<!-- Change file extension to .cgi -->
<!-- Replace "<option value=" with %AF0 -->
<!-- Replace "</option>" with %AF1 -->