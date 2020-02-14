/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <Corrade/Containers/Optional.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/String.h>

#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/MeshData.h"
// #include "Magnum/Trade/AbstractSceneConverter.h"
#include "Magnum/Trade/Implementation/converterSetOptions.h"

namespace Magnum {

}

using namespace Magnum;

int main(int argc, char** argv) {
    Utility::Arguments args;
    args.addArgument("input").setHelp("input", "input file")
        .addArgument("output").setHelp("output", "output file")
        .addOption("importer", "AnySceneImporter").setHelp("importer", "scene importer plugin")
        .addOption("converter", "AnySceneConverter").setHelp("converter", "scene converter plugin")
        .addOption("plugin-dir").setHelp("plugin-dir", "override base plugin dir", "DIR")
        .addOption('i', "importer-options").setHelp("importer-options", "configuration options to pass to the importer", "key=val,key2=val2,…")
        .addOption('c', "converter-options").setHelp("converter-options", "configuration options to pass to the converter", "key=val,key2=val2,…")
        .addBooleanOption("serialize").setHelp("serialize", "Serialize the imported data to a blob")
        .setGlobalHelp("Converts scenes of different formats.")
        .parse(argc, argv);

    PluginManager::Manager<Trade::AbstractImporter> importerManager{
        args.value("plugin-dir").empty() ? std::string{} :
        Utility::Directory::join(args.value("plugin-dir"), Trade::AbstractImporter::pluginSearchPaths()[0])};

    Containers::Pointer<Trade::AbstractImporter> importer = importerManager.loadAndInstantiate(args.value("importer"));
    if(!importer) {
        Debug{} << "Available importer plugins:" << Utility::String::join(importerManager.aliasList(), ", ");
        return 1;
    }

    if(!args.isSet("serialize")) {
        Error{} << "Sorry, only the --serialize option is currently implemented";
        return 6;
    }

    /* Set options, if passed */
    Trade::Implementation::setOptions(*importer, args.value("importer-options"));

    /* Open input file */
    if(!importer->openFile(args.value("input"))) {
        Error{} << "Cannot open file" << args.value("input");
        return 3;
    }

//     /* Load converter plugin */
//     PluginManager::Manager<Trade::AbstractSceneConverter> converterManager{
//         args.value("plugin-dir").empty() ? std::string{} :
//         Utility::Directory::join(args.value("plugin-dir"), Trade::AbstractImageConverter::pluginSearchPaths()[0])};
//     Containers::Pointer<Trade::AbstractSceneConverter> converter = converterManager.loadAndInstantiate(args.value("converter"));
//     if(!converter) {
//         Debug{} << "Available converter plugins:" << Utility::String::join(converterManager.aliasList(), ", ");
//         return 2;
//     }

    /* Set options, if passed */
//     Trade::Implementation::setOptions(*converter, args.value("converter-options"));
//
//     /* Save output file */
//     if(!converter->convertToFile(*importer, args.value("output"))) {
//     }

    Containers::Optional<Trade::MeshData> mesh = importer->mesh(0);
    if(!importer->meshCount() || !(mesh = importer->mesh(0))) {
        Error{} << "Cannot import mesh 0";
        return 4;
    }

    if(!Utility::Directory::write(args.value("output"), mesh->serialize())) {
        Error{} << "Cannot save file" << args.value("output");
        return 5;
    }
}
