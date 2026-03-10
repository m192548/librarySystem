/*
 Navicat Premium Data Transfer

 Source Server         : connect1
 Source Server Type    : MySQL
 Source Server Version : 80033 (8.0.33)
 Source Host           : localhost:3306
 Source Schema         : librarymanagersystem

 Target Server Type    : MySQL
 Target Server Version : 80033 (8.0.33)
 File Encoding         : 65001

 Date: 10/03/2026 10:09:45
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for books
-- ----------------------------
DROP TABLE IF EXISTS `books`;
CREATE TABLE `books`  (
  `id` int NOT NULL AUTO_INCREMENT COMMENT '图书ID（主键，自增）',
  `book_no` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL COMMENT '图书编号（自定义，如ISBN）',
  `book_name` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL COMMENT '图书名称',
  `author` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL COMMENT '作者',
  `publisher` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL COMMENT '出版社',
  `publish_date` date NOT NULL COMMENT '出版日期',
  `category` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL COMMENT '图书分类（如小说、计算机）',
  `stock` int NOT NULL DEFAULT 0 COMMENT '库存数量',
  `available_quantity` int NOT NULL DEFAULT 0 COMMENT '当前可借数量',
  `current_borrowed_count` int NOT NULL COMMENT '当前借出数量（借出还未归还的数量)',
  `total_borrowed_count` int NOT NULL DEFAULT 0 COMMENT '借阅总次数(总共借阅的次数)',
  `status` tinyint NOT NULL DEFAULT 1 COMMENT '状态：1-可借，0-不可借（损坏/下架）',
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE INDEX `book_no`(`book_no` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 25120 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = '图书表' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for borrow_record
-- ----------------------------
DROP TABLE IF EXISTS `borrow_record`;
CREATE TABLE `borrow_record`  (
  `id` int NOT NULL AUTO_INCREMENT COMMENT '记录ID（主键，自增）',
  `book_no` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL COMMENT '图书ID（关联book表）',
  `reader_no` varchar(10) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL COMMENT '读者ID（关联user表）',
  `is_return` enum('是','否') CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL DEFAULT '否' COMMENT '是否归还',
  `borrow_time` datetime NULL DEFAULT CURRENT_TIMESTAMP COMMENT '借阅时间',
  `return_time` datetime NULL DEFAULT NULL COMMENT '归还时间（NULL表示未归还）',
  `due_time` datetime NOT NULL COMMENT '应还日期',
  `deducted_Points` int NULL DEFAULT (0) COMMENT '应扣除的信誉积分',
  `fine` decimal(10, 2) NULL DEFAULT 0.00 COMMENT '逾期罚款（元）',
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `foreignKey1`(`book_no` ASC) USING BTREE,
  INDEX `foreignKey2`(`reader_no` ASC) USING BTREE,
  CONSTRAINT `foreignKey1` FOREIGN KEY (`book_no`) REFERENCES `books` (`book_no`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `foreignKey2` FOREIGN KEY (`reader_no`) REFERENCES `user` (`number`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE = InnoDB AUTO_INCREMENT = 11543 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = '借阅记录表' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for user
-- ----------------------------
DROP TABLE IF EXISTS `user`;
CREATE TABLE `user`  (
  `uid` int NOT NULL AUTO_INCREMENT,
  `number` varchar(10) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `account` varchar(15) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `password` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `nickname` varchar(40) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `permission` enum('admin','reader') CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL DEFAULT 'reader',
  `name` varchar(15) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `phone` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `email` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `create_time` datetime NOT NULL,
  `last_login_time` datetime NULL DEFAULT NULL,
  `borrow_count` int NOT NULL DEFAULT 0,
  `borrow_point` int NOT NULL DEFAULT 100,
  PRIMARY KEY (`uid`) USING BTREE,
  UNIQUE INDEX `number`(`number` ASC) USING BTREE,
  UNIQUE INDEX `number_2`(`number` ASC) USING BTREE,
  INDEX `uid`(`uid` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 196 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Procedure structure for batch_delete_books
-- ----------------------------
DROP PROCEDURE IF EXISTS `batch_delete_books`;
delimiter ;;
CREATE PROCEDURE `batch_delete_books`()
BEGIN
    DECLARE delete_count INT DEFAULT 0;
    DECLARE batch_size INT DEFAULT 1000;  -- 每批次删除1000条
    DECLARE total_delete INT DEFAULT 10000;  -- 总删除量

    -- 关闭自动提交，提升效率
    SET AUTOCOMMIT = 0;

    WHILE delete_count < total_delete DO
        DELETE FROM books 
        ORDER BY id  -- 按主键排序，避免随机锁表
        LIMIT batch_size;

        SET delete_count = delete_count + batch_size;
        COMMIT;  -- 每批次提交一次
        DO SLEEP(0.1);  -- 暂停0.1秒，降低数据库压力
    END WHILE;

    -- 恢复自动提交
    SET AUTOCOMMIT = 1;
END
;;
delimiter ;

-- ----------------------------
-- Procedure structure for generate_books_data
-- ----------------------------
DROP PROCEDURE IF EXISTS `generate_books_data`;
delimiter ;;
CREATE PROCEDURE `generate_books_data`()
BEGIN
    DECLARE i INT DEFAULT 1;
    DECLARE categories VARCHAR(50);
    DECLARE publishers VARCHAR(100);
    DECLARE status_val TINYINT;

    -- 分类列表
    SET @categories = '小说,计算机,历史,文学,科普,经济,法律,医学,艺术,教育';
    -- 出版社列表
    SET @publishers = '人民邮电出版社,机械工业出版社,清华大学出版社,北京大学出版社,中信出版社,商务印书馆,中华书局,上海译文出版社,译林出版社,广西师范大学出版社';

    -- 关闭自动提交，提升插入速度
    SET autocommit = 0;

    WHILE i <= 15000 DO
        -- 随机分类
        SET categories = SUBSTRING_INDEX(SUBSTRING_INDEX(@categories, ',', FLOOR(1 + RAND() * 10)), ',', -1);
        -- 随机出版社
        SET publishers = SUBSTRING_INDEX(SUBSTRING_INDEX(@publishers, ',', FLOOR(1 + RAND() * 10)), ',', -1);
        -- 随机状态：90% 为可借(1)，10% 为不可借(0)
        SET status_val = IF(RAND() < 0.9, 1, 0);

        INSERT INTO books (
            book_no,
            book_name,
            author,
            publisher,
            publish_date,
            category,
            stock,
            available_quantity,
            current_borrowed_count,
            total_borrowed_count,
            status
        ) VALUES (
            CONCAT('BK', LPAD(i, 5, "1000")), -- 图书编号：BK00000001 递增
            CONCAT('图书', i), -- 图书名称：图书1, 图书2...
            CONCAT('作者', FLOOR(1 + RAND() * 500)), -- 作者：作者1~500
            publishers, -- 随机出版社
            DATE_SUB(CURDATE(), INTERVAL FLOOR(RAND() * 365 * 20) DAY), -- 出版日期：近20年随机
            categories, -- 随机分类
            FLOOR(5 + RAND() * 50), -- 库存：5~54本
            FLOOR(5 + RAND() * 50), -- 当前可借：5~54本（可根据需要调整）
            FLOOR(0 + RAND() * 10), -- 当前借出：0~9本
            FLOOR(0 + RAND() * 100), -- 总借阅次数：0~99次
            status_val -- 状态：1-可借，0-不可借
        );

        SET i = i + 1;
    END WHILE;

    -- 提交事务
    COMMIT;
    -- 恢复自动提交
    SET autocommit = 1;
END
;;
delimiter ;

-- ----------------------------
-- Procedure structure for Insert100Books
-- ----------------------------
DROP PROCEDURE IF EXISTS `Insert100Books`;
delimiter ;;
CREATE PROCEDURE `Insert100Books`()
BEGIN
    DECLARE i INT DEFAULT 1;
    DECLARE bookNo VARCHAR(50);
    DECLARE bookName VARCHAR(100);
    DECLARE authorName VARCHAR(50);
    DECLARE publisherName VARCHAR(100);
    DECLARE publishDt DATE;
    DECLARE cate VARCHAR(50);
    DECLARE stockNum INT;
    
    -- 循环插入100条数据
    WHILE i <= 100 DO
        -- 生成随机图书编号（ISBN格式模拟）
        SET bookNo = CONCAT('978-7-', FLOOR(RAND()*90000+10000), '-', LPAD(i, 5, '0'));
        -- 生成随机图书名称
        SET bookName = CONCAT('测试图书_', i, '_', SUBSTRING(MD5(RAND()), 1, 8));
        -- 随机作者（模拟常见作者名）
        SET authorName = ELT(FLOOR(RAND()*10+1), '张三', '李四', '王五', '赵六', '钱七', '孙八', '周九', '吴十', '郑十一', '冯十二');
        -- 随机出版社
        SET publisherName = ELT(FLOOR(RAND()*8+1), '人民邮电出版社', '机械工业出版社', '清华大学出版社', '电子工业出版社', '北京大学出版社', '上海人民出版社', '浙江教育出版社', '中信出版社');
        -- 随机出版日期（近10年）
        SET publishDt = DATE_SUB(CURDATE(), INTERVAL FLOOR(RAND()*3650) DAY);
        -- 随机分类
        SET cate = ELT(FLOOR(RAND()*6+1), '小说', '计算机', '历史', '科普', '文学', '经管');
        -- 随机库存（1-50本）
        SET stockNum = FLOOR(RAND()*50+1);
        
        -- 插入数据，确保current_count = stockNum
        INSERT INTO books (
            book_no, book_name, author, publisher, publish_date, 
            category, stock, current_count, borrow_count, status
        ) VALUES (
            bookNo, bookName, authorName, publisherName, publishDt,
            cate, stockNum, stockNum, 0, 1  -- current_count = stockNum，借阅次数初始为0，状态可借
        ) ON DUPLICATE KEY UPDATE book_no = book_no;  -- 避免book_no重复时报错
        
        SET i = i + 1;
    END WHILE;
END
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table borrow_record
-- ----------------------------
DROP TRIGGER IF EXISTS `trg_after_borrow_insert`;
delimiter ;;
CREATE TRIGGER `trg_after_borrow_insert` AFTER INSERT ON `borrow_record` FOR EACH ROW BEGIN
    DECLARE available_count INT;
    SELECT available_quantity INTO available_count 
    FROM books 
    WHERE book_no = NEW.book_no FOR UPDATE;
    
    IF available_count <= 0 THEN
        SIGNAL SQLSTATE '45000' 
        SET MESSAGE_TEXT = '图书库存不足，无法借阅';
    END IF;
    
    UPDATE books
    SET current_borrowed_count = current_borrowed_count + 1,
        available_quantity = available_quantity - 1,
        total_borrowed_count = total_borrowed_count + 1
    WHERE book_no = NEW.book_no;

    UPDATE `user`
    SET borrow_count = borrow_count + 1 
    WHERE number = NEW.reader_no;
END
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table borrow_record
-- ----------------------------
DROP TRIGGER IF EXISTS `trg_calculate_penalty`;
delimiter ;;
CREATE TRIGGER `trg_calculate_penalty` BEFORE UPDATE ON `borrow_record` FOR EACH ROW BEGIN
    DECLARE day_diff FLOAT;  -- 存储return_time和due_time的天数差值
    DECLARE rounded_diff INT; -- 存储向上取整后的天数差值
    
    -- 仅当return_time不为空时执行计算逻辑
    IF NEW.return_time IS NOT NULL THEN
        -- 计算两个时间的天数差值（TIMESTAMPDIFF单位为秒，转换为天数）
        SET day_diff = TIMESTAMPDIFF(SECOND, NEW.due_time, NEW.return_time) / (24 * 3600);
        UPDATE books SET available_quantity = available_quantity+1,current_borrowed_count=current_borrowed_count-1 wHERE book_no = NEW.book_no;
        
        -- 仅处理逾期的情况（return_time > due_time）
        IF day_diff > 0 THEN
            -- 向上取整差值（例如1.2天取2，2.8天取3）
            SET rounded_diff = CEIL(day_diff);
            
            -- 计算扣分：小于1天扣5分，大于1天则向上取整差值*5
            IF day_diff < 1 THEN
                SET NEW.deducted_Points = 5;
            ELSE
                SET NEW.deducted_Points = rounded_diff * 5;
            END IF;
            
            -- 计算罚款：差值>3天时，fine=10*(差值/3)
            IF day_diff > 3 THEN
                SET NEW.fine = 10 * (day_diff / 3);
            ELSE
                -- 未超3天则罚款置空（或根据需求设为0）
                SET NEW.fine = 0;
            END IF;
        ELSE
            -- 未逾期（return_time <= due_time），重置扣分和罚款
            SET NEW.deducted_Points = 0;
            SET NEW.fine = 0;
        END IF;
    END IF;
END
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table borrow_record
-- ----------------------------
DROP TRIGGER IF EXISTS `trg_updateBook_delete`;
delimiter ;;
CREATE TRIGGER `trg_updateBook_delete` AFTER DELETE ON `borrow_record` FOR EACH ROW UPDATE books
    SET current_borrowed_count = current_borrowed_count-1,
    available_quantity = available_quantity+1 
		WHERE book_no= OLD.book_no
;;
delimiter ;

SET FOREIGN_KEY_CHECKS = 1;
