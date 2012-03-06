# -*- coding: utf-8 -*-
#!/usr/bin/env python

import salome_pluginsmanager

DEMO_IS_ACTIVATED = True

from minmax_plugin import *
 
# register the function in the plugin manager
if DEMO_IS_ACTIVATED:
  salome_pluginsmanager.AddFunction('Get min or max value of control',
      'Get min or max value of control',
      minmax)
