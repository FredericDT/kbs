<?php
// $Id$
require_once '..'.DIRECTORY_SEPARATOR.'funcs.php';
require_once 'config.modules.php';

login_init();

$GLOBAL_MODULES = new Modules ();

class Modules {
    var $mList;
    
    function Modules () {
        global $smth_modules;
        
        $this->mList = array ();
        foreach ($smth_modules as $_m) {
            $buf = new Module ();
            $buf->makeModuleParams ($_m);
            if ($buf->mid)
                $this->mList[$buf->mid] = $buf;
            unset ($buf);
        }
    }
    
    function isMidExist($mid) {
        $mid=intval($mid);
        if(!$mid) return false;
        return array_key_exists($mid,$this->mList);
    }
}

class Module{
    var $mid;
    var $name;
    var $access;
    var $version;
    var $innerhost;
    var $innerport;
    var $outerhost;
    var $outerport;
    var $xmlrpcsuser;
    var $xmlrpcspass;
    var $xmlrpcstype;
    var $xmlrpcspath;
    var $xmlrpcuser;
    var $xmlrpcpass;
    var $xmlrpctype;
    var $managepath;
    var $path;
    var $logo;
    var $type;
    var $typeName;
    var $desc;
    var $public;
    var $kernel;
    
    function Module () {
        
    }
    
    function makeModuleParams ($array) {
        $this->mid = $array['mid'];
        $this->name = $array['name'];
        $this->access = $array['access'];
        $this->version = $array['version'];
        $this->innerhost = $array['innerhost'];
        $this->innerport = $array['innerport'];
        $this->outerhost = $array['outerhost'];
        $this->outerport = $array['outerport'];
        $this->xmlrpcsuser = $array['xmlrpcsuser'];
        $this->xmlrpcspass = $array['xmlrpcspass'];
        $this->xmlrpcspath = $array['xmlrpcspath'];
        $this->xmlrpcstype = $array['xmlrpcstype'];
        $this->xmlrpcuser = $array['xmlrpcuser'];
        $this->xmlrpcpass = $array['xmlrpcpass'];
        $this->xmlrpctype = $array['xmlrpctype'];
        $this->path = $array['path'];
        $this->managepath = $array['managepath'];
        $this->logo = $array['logo'];
        $this->type = $array['type'];
        
        $this->public = $this->havePerm (MODULE_ACCESS_PUBLIC);
        $this->kernel = $this->havePerm (MODULE_ACCESS_KERNEL);
        
        settype ($this->mid, 'integer');
    }

    function resetAccess () {
        $this->access = MODULE_ACCESS_NONE;    
    }

    function addAccess ($access) {
        settype ($access, 'integer');
        $this->access |= $access;    
    }


    function isPublic () {
        return $this->public;    
    }

    function isKernel () {
        return $this->kernel;    
    }

    function isHidden () {
        return $this->havePerm (MODULE_ACCESS_HIDDEN);    
    }

    function isDisabled () {
        return $this->havePerm (MODULE_ACCESS_DISABLE);    
    }

    function isAnonymous () {
        return $this->havePerm (MODULE_ACCESS_ANONYMOUS);    
    }

    function havePerm ($perm) {
        return ($this->access & $perm);
    }  
}

function get_remote_host () {
        return $_SERVER['REMOTE_ADDR'];
}
?>