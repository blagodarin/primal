// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#define PRIMAL_JOIN_(left, right) left##right
#define PRIMAL_JOIN(left, right) PRIMAL_JOIN_(left, right)
