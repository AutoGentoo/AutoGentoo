#!/bin/bash

PKG_COUNT=$(find data/cportage-repo/ -print | grep -v Manifest.gz | grep -cE 'data/cportage-repo/[0-9|a-z|\-]+/.+')
echo "Found ${PKG_COUNT} packages"