/*
 Navicat Premium Data Transfer

 Source Server         : cmg_connection
 Source Server Type    : MySQL
 Source Server Version : 80016
 Source Host           : localhost:3306
 Source Schema         : jwt

 Target Server Type    : MySQL
 Target Server Version : 80016
 File Encoding         : 65001

 Date: 21/06/2020 09:33:06
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for accounts
-- ----------------------------
DROP TABLE IF EXISTS `accounts`;
CREATE TABLE `accounts`  (
  `uid` varchar(10) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `password` varchar(16) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`uid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for bags
-- ----------------------------
DROP TABLE IF EXISTS `bags`;
CREATE TABLE `bags`  (
  `uid` varchar(10) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `GB` int(11) NOT NULL DEFAULT 0 COMMENT '玩家的金钱数目',
  `气血丸小号` int(11) NOT NULL DEFAULT 0 COMMENT '气血丸小号',
  `气血丸中号` int(11) NOT NULL DEFAULT 0 COMMENT '气血丸中号',
  `气血丸大号` int(11) NOT NULL DEFAULT 0 COMMENT '气血丸大号',
  `经验书小号` int(11) NOT NULL DEFAULT 0 COMMENT '经验书小号',
  `经验书中号` int(11) NOT NULL DEFAULT 0 COMMENT '经验书中号',
  `经验书大号` int(11) NOT NULL DEFAULT 0 COMMENT '经验书大号',
  PRIMARY KEY (`uid`) USING BTREE,
  CONSTRAINT `bags_ibfk_1` FOREIGN KEY (`uid`) REFERENCES `accounts` (`uid`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for friends
-- ----------------------------
DROP TABLE IF EXISTS `friends`;
CREATE TABLE `friends`  (
  `uid` varchar(10) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `friend1` varchar(10) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `friend2` varchar(10) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `friend3` varchar(10) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `friend4` varchar(10) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `friend5` varchar(10) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  PRIMARY KEY (`uid`) USING BTREE,
  CONSTRAINT `friends_ibfk_1` FOREIGN KEY (`uid`) REFERENCES `accounts` (`uid`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for players
-- ----------------------------
DROP TABLE IF EXISTS `players`;
CREATE TABLE `players`  (
  `uid` varchar(10) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '用户uid',
  `lv` int(11) NOT NULL DEFAULT 1 COMMENT '当前等级',
  `exp` int(11) NOT NULL DEFAULT 0 COMMENT '当前存储的经验。每要升一级，需要经验为当前等级*100。升级完成后，当前存储的经验减去升级花费的经验。',
  `qi` int(11) NOT NULL DEFAULT 50 COMMENT '气血值。初始值为50。每次战斗减5点',
  `hp` int(11) NOT NULL DEFAULT 1 COMMENT '血量=hp*20',
  `ap` int(11) NOT NULL DEFAULT 1 COMMENT '攻击=ap*10',
  `dp` int(11) NOT NULL DEFAULT 1 COMMENT '防御=dp*5',
  `trainingRoomStartTime` datetime(0) NULL DEFAULT NULL COMMENT '练功房修炼开始的时间。4小时候获得1000+lv*20点经验和50+lv点气血',
  `skillPoint` int(11) NOT NULL DEFAULT 0 COMMENT '待分配的技能点，每升一级得3点',
  PRIMARY KEY (`uid`) USING BTREE,
  CONSTRAINT `players_ibfk_1` FOREIGN KEY (`uid`) REFERENCES `accounts` (`uid`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for shop
-- ----------------------------
DROP TABLE IF EXISTS `shop`;
CREATE TABLE `shop`  (
  `id` int(11) NOT NULL,
  `itemName` varchar(20) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `cost` int(11) NOT NULL,
  `addQi` int(11) NOT NULL,
  `addEXP` int(11) NOT NULL,
  `comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Dynamic;

SET FOREIGN_KEY_CHECKS = 1;
