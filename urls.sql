SET NAMES utf8;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
--  Table structure for `url_1`
-- ----------------------------
DROP TABLE IF EXISTS `url_1`;
CREATE TABLE `url_1` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `url` varchar(2048) DEFAULT NULL,
  `created` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`),
  FULLTEXT KEY `url` (`url`)
) ENGINE=MyISAM AUTO_INCREMENT=1042 DEFAULT CHARSET=latin1;

