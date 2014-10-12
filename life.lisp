(setf *row* 20 *col* 20)

(defun gen-rand-field ()
  (loop repeat *row* collect (loop repeat *col* collect (random 2))))

(defun moore-neighbour (array-field r c)
  (let ((ri (mapcar (lambda (x) (mod (+ *row* r x) *row*)) '(-1 -1 0 1 1 1 0 -1)))
	(ci (mapcar (lambda (y) (mod (+ *col* c y) *col*)) '(0 1 1 1 0 -1 -1 -1))))
    (loop for x in ri for y in ci sum (aref array-field x y))))

(defun gen-next-field (field)
  (let ((array-field (make-array `(,*row* ,*col*) :initial-contents field)))
    (loop for i below *row*
	  collect (loop for j below *col*
			collect (let ((n (moore-neighbour array-field i j))
				      (s (aref array-field i j)))
				  (cond ((= n 3) 1) ((= n 2) s) (t 0)))))))

(defun print-field (field g)
  (format t "+~{~a~}+~%" (loop repeat *col* collect "--"))
  (loop for l in field
	do (format t "|~{~a~}|~%" (loop for x in l
					collect (if (= 1 x) "[]" "  "))))
  (format t "+~{~a~}+~%Generation: ~a~%" (loop repeat *col* collect "--") g))

(defun life (&optional (field (gen-rand-field)) (g 0))
  (print-field field g)
  (sleep 1)
  (life (gen-next-field field) (1+ g)))
