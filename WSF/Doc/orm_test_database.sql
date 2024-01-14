
DROP DATABASE IF EXISTS ORM_TEST;
CREATE DATABASE ORM_TEST;

USE ORM_TEST;
SET SESSION TRANSACTION ISOLATION LEVEL SERIALIZABLE;
SET AUTOCOMMIT = 0;

/*****************************************************************************/
CREATE TABLE tblOrmIDs
(
   name   	VARCHAR(64) NOT NULL,
   next_id  INT NOT NULL,

   UNIQUE INDEX (name)
)
TYPE = InnoDB;

/*****************************************************************************/
CREATE TABLE tblOrmObjects
(
   id        INT NOT NULL PRIMARY KEY,
   orm_class VARCHAR(128) NOT NULL,

   UNIQUE INDEX (id)
)
TYPE = InnoDB;

/*****************************************************************************/
CREATE TABLE tblColors
(
   id        INT NOT NULL PRIMARY KEY,
   r         INT NOT NULL,
   g         INT NOT NULL,
   b         INT NOT NULL,
   name      VARCHAR (255) NOT NULL,

   UNIQUE INDEX (id),
   FOREIGN KEY (id) REFERENCES tblOrmObjects (id) ON DELETE CASCADE
)
TYPE = InnoDB;

/*****************************************************************************/
CREATE TABLE tblShapes
(
   id        INT NOT NULL PRIMARY KEY,
   x         INT NOT NULL,
   y         INT NOT NULL,
   parent_id INT,
   color_id  INT,
   name      VARCHAR (255) NOT NULL,

   UNIQUE INDEX (id),
   INDEX (parent_id),
   INDEX (color_id),
   
   FOREIGN KEY (id) REFERENCES tblOrmObjects (id) ON DELETE CASCADE,
   FOREIGN KEY (parent_id) REFERENCES tblShapes (id) ON DELETE RESTRICT,
   FOREIGN KEY (color_id) REFERENCES tblColors (id) ON DELETE SET NULL
)
TYPE = InnoDB;

/*****************************************************************************/
CREATE TABLE tblTriangles
(
   id        INT NOT NULL PRIMARY KEY,
   alpha     INT NOT NULL,
   beta      INT NOT NULL,
   base      INT NOT NULL,

   UNIQUE INDEX (id),
   FOREIGN KEY (id) REFERENCES tblShapes (id) ON DELETE CASCADE
)
TYPE = InnoDB;

/*****************************************************************************/
CREATE TABLE tblAreas
(
   id            INT NOT NULL PRIMARY KEY,
   border_width  INT NOT NULL,

   UNIQUE INDEX (id),
   FOREIGN KEY (id) REFERENCES tblShapes (id) ON DELETE CASCADE
)
TYPE = InnoDB;


INSERT INTO tblOrmIDs VALUES ("orm_object_id", 1);
COMMIT;

