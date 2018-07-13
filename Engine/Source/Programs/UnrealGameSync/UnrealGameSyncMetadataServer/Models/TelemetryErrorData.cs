﻿// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace UnrealGameSyncMetadataServer.Models
{
	public class TelemetryErrorData
	{
		public enum TelemetryErrorType
		{
			Crash,
		}
		public int Id;
		public TelemetryErrorType Type;
		public string Text;
		public string UserName;
		public string Project;
		public DateTime Timestamp;
		public string Version;
		public string IpAddress;
	}
}