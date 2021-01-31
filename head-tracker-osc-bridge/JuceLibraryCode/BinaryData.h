/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   male_head_obj;
    const int            male_head_objSize = 259542;

    extern const char*   axis_png;
    const int            axis_pngSize = 1653;

    extern const char*   osc_png;
    const int            osc_pngSize = 2274;

    extern const char*   serial_png;
    const int            serial_pngSize = 2443;

    extern const char*   Tbold_ttf;
    const int            Tbold_ttfSize = 61208;

    extern const char*   segoeui_ttf;
    const int            segoeui_ttfSize = 841924;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 6;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
