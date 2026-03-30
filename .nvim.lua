vim.g.project = {debug = {}}

local root = vim.fn.fnamemodify(
  debug.getinfo(1, "S").source:sub(2),
  ":p:h"
)

local project = require("anoukh.project")


project.debug.exe = root .. "\\build\\debug\\bin\\exe.exe"
project.build = "build-debug.bat"
