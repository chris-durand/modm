#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

from pathlib import Path
from jinja2 import Environment
import graphviz as gv
import re

try:
    import lbuild
except:
    import sys
    from os.path import join, dirname, realpath
    rootpath = join(dirname(realpath(__file__)), "../../../library-builder")
    sys.path.append(rootpath)
    import lbuild

def get_modules():
    lbuild.logger.configure_logger(0)
    config = lbuild.config.Configuration.parse_configuration("{}/tools/scripts/project.xml".format(root))
    parser = lbuild.parser.Parser()
    parser.load_repositories(config, [])
    commandline_options = config.format_commandline_options(["modm:target=stm32f469nih"])
    repo_options = parser.merge_repository_options(config.options, commandline_options)
    modules = parser.prepare_repositories(repo_options)

    selected_modules = lbuild.module.resolve_modules(modules, [":**"])
    build_modules = parser.resolve_dependencies(modules, selected_modules)
    module_options = parser.merge_module_options(build_modules, config.options)

    return build_modules, module_options

def extract(text, key):
    return re.search(r"# {0}\n(.*?)\n# /{0}".format(key), text, flags=re.DOTALL | re.MULTILINE).group(1)

def replace(text, key, content):
    return re.sub(r"# {0}.*?# /{0}".format(key), "# {0}\n{1}\n# /{0}".format(key, content), text, flags=re.DOTALL | re.MULTILINE)

def ref_name(name):
    return name.replace(":", "_").replace(".", "_")

def node_name(name):
    return name.replace(":", ":\n")

def url_name(name):
    return name.replace(":", "-").replace(".", "-")

def split_description(descr):
    description = []
    title = None
    for line in descr.splitlines():
        if title is None and line.startswith("#"):
            title = line.replace("#", "").strip()
            continue
        description.append(line)
    if len(description) == 1:
        title = description[0]
        description = []
    return title, "\n".join(description)

# All the paths
root = Path.cwd().parents[1]
config_path = root / "docs/mkdocs.yml"
modules_in_path = root / "docs/module.md.in"
modules_path = root / "docs/src/reference/module"

omodules, ooptions = get_modules()
omodules.sort(key=lambda m: m.fullname)

modules = []
for m in omodules:
    title, descr = split_description(m.description)
    mprops = {"name": m.fullname,
              "title": title,
              "description": descr,
              "ref": url_name(m.fullname),
              "dependencies": sorted([d.fullname for d in m.dependencies]),
              "options": []}
    for o in m.options.values():
        title, descr = split_description(o.description)
        oprops = {"name": o.fullname,
                  "title": title,
                  "description": descr,
                  "default": str(o.value) if o.value else None,
                  "possibles": o.values_hint()}
        mprops["options"].append(oprops)
    modules.append(mprops)

for m in modules:
    m["dependents"] = []
    for d in modules:
        if m["name"] in d["dependencies"]:
            m["dependents"].append(d["name"])
    m["dependents"].sort()

    graph = gv.Digraph(name=m["name"],
                       format="svg",
                       graph_attr={"rankdir": "BT"},
                       node_attr={"style": "filled,solid", "shape": "box"})
    graph.node(ref_name(m["name"]), node_name(m["name"]), style="filled,bold")

    for mod in (m["dependencies"] + m["dependents"]):
        graph.node(ref_name(mod), node_name(mod), URL=("../" + url_name(mod)))
    for mod in m["dependencies"]:
        graph.edge(ref_name(m["name"]), ref_name(mod))
    for mod in m["dependents"]:
        graph.edge(ref_name(mod), ref_name(m["name"]))

    m["graph"] = graph.pipe().decode('utf-8')

modtable = []
for m in modules:
    module_text = Environment().from_string(modules_in_path.read_text()).render({"module": m})
    subpath = modules_path / (url_name(m["name"]) + ".md")
    if subpath.read_text() != module_text:
        subpath.write_text(module_text)
        print("Writing to", subpath)
    modtable.append("      - {}: {}".format(m["name"], subpath.relative_to(root / "docs/src")))


modtable = "\n".join(modtable)
config = config_path.read_text()
if extract(config, "moduletable") != modtable:
    config = replace(config, "moduletable", modtable)
    config_path.write_text(config)

