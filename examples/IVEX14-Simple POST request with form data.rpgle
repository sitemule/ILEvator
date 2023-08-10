**FREE

///
// ILEvator Example : Simple POST request with form data
//
// This example does a POST request with data form encoded (a.k.a. percent
// encoding). 
///


ctl-opt dftactgrp(*no) actgrp(*new) debug(*yes) bnddir('ILEVATOR') main(main);
ctl-opt copyright('Sitemule.com  (C), 2022-2023');


/define RPG_HAS_OVERLOAD
/include 'ilevator.rpgle'
/include 'form.rpginc'
/include 'varchar.rpginc'


dcl-proc main;
    dcl-s string varchar(IV_BUFFER_SIZE:4) ccsid(1208);
    dcl-ds formData likeds(LVARPUCHAR_t);
    
    formData = iv_form_of(
        'firstName' : 'Mihael' :
        'lastName' : 'Schmidt' :
        'email' : 'mihael@rpgnextgen.com'
    );
    
    string = %str(formData.String : formData.Length);
    
    string = iv_post('http://localhost/registration': string : 'application/x-www-form-urlencoded');

    iv_joblog(string);
    
    on-exit;
        iv_form_free(formData);
end-proc;
