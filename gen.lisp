(eval-when (:compile-toplevel :execute :load-toplevel)
  (ql:quickload :cl-cpp-generator))

(in-package :cl-cpp-generator)

(defmacro e (&body body)
  `(statements (<< "std::cout" ,@(loop for e in body collect
                                      (cond ((stringp e) `(string ,e))
                                            (t e))) "std::endl")))

(defmacro er (&body body)
  `(statements (<< "std::cerr" ,@(loop for e in body collect
                                      (cond ((stringp e) `(string ,e))
                                            (t e))) "std::endl")))


(defun replace-all (string part replacement &key (test #'char=))
"Returns a new string in which all the occurences of the part 
is replaced with replacement."
    (with-output-to-string (out)
      (loop with part-length = (length part)
            for old-pos = 0 then (+ pos part-length)
            for pos = (search part string
                              :start2 old-pos
                              :test test)
            do (write-string string out
                             :start old-pos
                             :end (or pos (length string)))
            when pos do (write-string replacement out)
            while pos))) 


(defun dox (&key brief usage params return)
  `(
    (raw " ")
    (raw ,(format nil "//! @brief ~a" brief)) (raw "//! ")
    (raw ,(format nil "//! @usage ~a" usage)) (raw "//! ")
    ,@(loop for (name desc) in params collect
         `(raw ,(format nil "//! @param ~a ~a" name desc)))
    (raw "//! ")
    (raw ,(format nil "//! @return ~a" return))
    (raw " ")))


(defparameter *drm-facts*
  `((10 "")))


(defmacro with-open-fstream ((f fn &key (dir "/dev/shm")) &body body)
  `(let ((,f :type "std::ofstream" :ctor (comma-list (string ,(format nil "~a/~a" dir fn)))))
     ,@body))
(progn
  (let* ((code `(with-compilation-unit
		    (with-compilation-unit
			(raw "//! \\file main.cpp Draw to screen using linux direct rendering manager"))
					
		  (include <drm/drm.h>)
		  (include <drm/drm_mode.h>)
		  (include <fstream>) 
		  (include <cstring>) ;; memset
		  (include "cxxopts.hpp")
		  (include <iomanip>) ;; setw
		  (raw " ")
		  (raw "//! \\mainpage Draw using direct rendering manager")
		  (raw "//! \\section Introduction")
		  (raw "//! This repository contains a minimal program to draw to a linux screen. See http://betteros.org/tut/graphics1.php for a tutorial this code is based on.")
		  (raw "//! \\section Dependencies ")
		  (raw "//! - Linux kernel with DRM driver")
		  (raw " ")
		  (raw "//! - cxxopts for commandline argument parsing.")
		  (raw " ")
		  (raw "//! - sbcl to generate c++ code")
		  (raw "//! - cmake to configure for build")
		  (raw "//! - g++ to compile c++ code")
		  (raw " ")
		  (raw "//! - For the documentation (optional for use):")
		  (raw "//!   + doxygen")
		  (raw " ")
		  (raw "//! - The C++ code can be opened as a cmake project in kdevelop, clion or qtcreator (order of my preference).")
		  (raw " ")
		  (raw "//! \\section References ")
		  ,@(loop for i from 1 and e in '("http://betteros.org/tut/graphics1.php")
		       collect
			 `(raw ,(format nil "//! ~a. ~a" i e)))
		  
		  
		  ,@(dox :brief "main function"
			 :usage "parse command line parameters and draw to screen"
			 :params '((argc "input number of command line arguments")
				   (argv "input"))
			 :return "Integer")
		  		  
		  (function (main ((argc :type int)
				   (argv :type char**)) int)

			    
			    
			    
			    (with-compilation-unit 
				(raw "try")
			      (let ((options :type "cxxopts::Options" :ctor (comma-list (string "drm_draw") (string "draw to drm device"))))
				(with-compilation-unit
				    (raw "options.add_options()")
				  (raw "(\"h,help\", \"Print help\")")
				  (raw "(\"r,rate\",\"frame rate (Hz,double)\",cxxopts::value<double>()->default_value(\"60.0\"));"))
				(funcall options.parse argc argv)
				
				(if (funcall options.count (string "help"))
				    (statements
				     (macroexpand (e (funcall options.help)))
				     (funcall exit 0)))
				(macroexpand (e "requested frame rate = "
						(funcall "options[\"rate\"].as<double>")
						" Hz"))
				)
			      
			      (raw "catch (const cxxopts::OptionException& e)")
			      (let ()
				(macroexpand (e "error parsing options: " (funcall e.what)))
				(funcall exit 1)))
			    
			    (return 0)))))
    (write-source "stage/cl-gen-graphics-drm/source/main" "cpp" code)))


