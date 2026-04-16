#!/bin/bash

# when is the uni going to learn about the submitting git diffs instead
git diff --name-only starting-point HEAD | xargs zip SOFTENG370_A1_psin539.zip
