;;
;; testing macro expansion
;;

(add-load-path "../lib")
(require "test")

;; strip off syntactic information from identifiers in the macro output.
(define (unident form)
  (cond
   ((identifier? form) (identifier->symbol form))
   ((pair? form) (cons (unident (car form)) (unident (cdr form))))
   ((vector? form)
    (list->vector (map unident (vector->list form))))
   (else form)))

(define-macro (test-macro msg expect form)
  `(test ,msg ',expect (lambda () (unident (%macro-expand ,form)))))

;; taken from R5RS section 7.3
(define-syntax %cond
  (syntax-rules (else =>)
    ((cond (else result1 result2 ...))
     (begin result1 result2 ...))
    ((cond (test => result))
     (let ((temp test))
       (if temp (result temp))))
    ((cond (test => result) clause1 clause2 ...)
     (let ((temp test))
       (if temp
           (result temp)
           (%cond clause1 clause2 ...))))
    ((cond (test)) test)
    ((cond (test) clause1 clause2 ...)
     (let ((temp test))
       (if temp temp (%cond clause1 clause2 ...))))
    ((cond (test result1 result2 ...))
     (if test (begin result1 result2 ...)))
    ((cond (test result1 result2 ...) clause1 clause2 ...)
     (if test (begin result1 result2 ...) (%cond clause1 clause2 ...)))
    ))

(define-syntax %letrec
  (syntax-rules ()
    ((_ ((var1 init1) ...) body ...)
     (%letrec "generate_temp_names"
              (var1 ...)
              ()
              ((var1 init1) ...)
              body ...))
    ((_ "generate_temp_names" () (temp1 ...) ((var1 init1) ...) body ...)
     (let ((var1 :undefined) ...)
       (let ((temp1 init1) ...)
         (set! var1 temp1) ...
         body ...)))
    ((_ "generate_temp_names" (x y ...) (temp ...) ((var1 init1) ...) body ...)
     (%letrec "generate_temp_names"
              (y ...)
              (newtemp temp ...)
              ((var1 init1) ...)
              body ...))))

(define-syntax %do
  (syntax-rules ()
    ((_ ((var init step ...) ...)
        (test expr ...)
        command ...)
     (letrec
         ((loop
           (lambda (var ...)
             (if test
                 (begin
                   (if #f #f)
                   expr ...)
                 (begin
                   command
                   ...
                   (loop (%do "step" var step ...)
                         ...))))))
       (loop init ...)))
    ((_ "step" x)
     x)
    ((_ "step" x y)
     y)))

;; test code
(test-macro "%cond" (begin a) (%cond (else a)))
(test-macro "%cond" (begin a b c) (%cond (else a b c)))
(test-macro "%cond" (let ((temp a)) (if temp (b temp))) (%cond (a => b)))
(test-macro "%cond" (let ((temp a)) (if temp (b temp) (%cond c))) (%cond (a => b) c))
(test-macro "%cond" (let ((temp a)) (if temp (b temp) (%cond c d))) (%cond (a => b) c d))
(test-macro "%cond" (let ((temp a)) (if temp (b temp) (%cond c d e))) (%cond (a => b) c d e))
(test-macro "%cond" a (%cond (a)))
(test-macro "%cond" (let ((temp a)) (if temp temp (%cond b))) (%cond (a) b))
(test-macro "%cond" (let ((temp a)) (if temp temp (%cond b c))) (%cond (a) b c))
(test-macro "%cond" (if a (begin b)) (%cond (a b)))
(test-macro "%cond" (if a (begin b c d)) (%cond (a b c d)))
(test-macro "%cond" (if a (begin b c d) (%cond e f g)) (%cond (a b c d) e f g))

;; test for higiene
(test "%cond" '(if a (begin => b))
      (lambda () (let ((=> #f)) (unident (%macro-expand (%cond (a => b)))))))
(test "%cond" '(if else (begin z))
      (lambda () (let ((else #t)) (unident (%macro-expand (%cond (else z)))))))

;; Note: if you "unident" the expansion result of %letrec, you see a symbol
;; "newtemp" appears repeatedly in the let binding, seemingly expanding
;; into invalid syntax.  Internally, however, those symbols are treated 
;; as identifiers with the correct identity, so the expanded code works
;; fine (as tested in the second test).
(test-macro "%letrec"
            (let ((a :undefined)
                  (c :undefined))
              (let ((newtemp b)
                    (newtemp d))
                (set! a newtemp)
                (set! d newtemp)
                e f g))
            (%letrec ((a b) (c d)) e f g))
(test "%letrec" '(1 2 3)
      (lambda () (%letrec ((a 1) (b 2) (c 3)) (list a b c))))

(newline)
