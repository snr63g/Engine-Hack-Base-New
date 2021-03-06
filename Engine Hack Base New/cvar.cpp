#include "Main.h"

CVARlist cvar;

#pragma warning(disable:4244)

void script_exec(char* filename)
{
	string file = szDirFile(filename);
	cmd.execFile(file.c_str());
}

void func_exec() { script_exec( cmd.argC(1) ); }

void func_alias()
{
	const char* name = cmd.argC(1);
	string& content = cmd.argS(2);
	cmd.AddAlias(name,content);
}

void func_echo()
{
	g_Engine.Con_Printf("%s\n",cmd.argC(1));
}

void set_cvar()
{
	char set_cvars[56];
	if ( cmd.names.find(g_Engine.Cmd_Argv(1)) )
	{
		sprintf_s(set_cvars,"%s %s",g_Engine.Cmd_Argv(1),g_Engine.Cmd_Argv(2));
		cmd.exec(set_cvars);
	}
	else
		cmd.exec(g_Engine.Cmd_Argv(1));
}

void CVARlist::init()
{
	memset((char*)this, 0, sizeof(*this));
	#define REGISTER_CVAR_FLOAT(name,defaultvalue) cmd.AddCvarFloat(#name, &##name );name=defaultvalue##f;
	#define REGISTER_CVAR_INT(name,defaultvalue) cmd.AddCvarInt(#name, &##name );name=defaultvalue;
	#define REGISTER_COMMAND(name) cmd.AddCommand( #name, func_##name);

	REGISTER_COMMAND(exec)
	REGISTER_COMMAND(echo)
	REGISTER_COMMAND(alias)

	// �������� �������� ���� ��� ����
	REGISTER_CVAR_INT(cvar_test,1);

	g_Engine.pfnAddCommand("set",set_cvar);
}

void HlEngineCommand(const char* command)
{
	if(!g_Engine.pfnClientCmd) { return; }
	g_Engine.pfnClientCmd( const_cast<char*>(command) );
}

void HandleCvarInt(char* name, int* value)
{
	char* arg1 = cmd.argC(1); 
	if (!strcmp(arg1,"change"))
	{
		if(*value) *value=0; 
		else *value=1;
		return;
	}
	if (!strcmp(arg1,"up")){*value += cmd.argI(2);return;}
	if (!strcmp(arg1,"down")){*value -= cmd.argI(2);return;}
	if (!strcmp(arg1,"hide")){*value = cmd.argI(2);return;}
	if (!*arg1)
	{
		g_Engine.Con_Printf( "CVAR %s = %i\n",name,*value); 
		return;
	}
	*value = cmd.argI(1);
}

void HandleCvarFloat(char* name, float* value)
{
	char* arg1 = cmd.argC(1);  
	if (!strcmp(arg1,"change"))
	{
		if(*value) *value=0; 
		else *value=1;
		return;
	}
	if (!strcmp(arg1,"up")){*value += cmd.argF(2);return;}
	if (!strcmp(arg1,"down")){*value -= cmd.argF(2);return;}
	if (!strcmp(arg1,"hide")){*value = cmd.argI(2);return;}
	if (!*arg1)
	{
		g_Engine.Con_Printf("CVAR %s = %f\n",name,*value);
		return;
	}
	*value = cmd.argF(1);
}

bool isHlCvar(char* name)
{
	if(!g_Engine.pfnGetCvarPointer) { return false; }
	cvar_s* test = g_Engine.pfnGetCvarPointer(name);
	return (test!=NULL);
}

bool HandleHlCvar(char* name)
{
	if(!g_Engine.pfnGetCvarPointer) { return false; }
	cvar_s* ptr = g_Engine.pfnGetCvarPointer(name);
	if(!ptr) { return false; }
	HandleCvarFloat(name,&ptr->value);
	return true;
}