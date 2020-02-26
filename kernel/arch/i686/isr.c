#include "isr.h"

#define make_isr(vecn, excep, irq)                \
    extern void isr_##vecn(void);                 \
    asm(                                          \
    ".globl isr_" #vecn "\n"                      \
    "isr_" #vecn ":\n"                            \
    "    pushal\n"                                \
    "    pushw $" #irq "\n"                       \
    "    pushw $" #excep "\n"                     \
    "    pushl $" #vecn "\n"                      \
    "    call general_interrupt_handler\n"        \
    "    add $0x8, %esp\n"                        \
    "    popal\n"                                 \
    "    iret\n");

#define get_isr(vecn) (uintptr_t) isr_##vecn


make_isr(0, 0, -1);
make_isr(1, 1, -1);
make_isr(2, 2, -1);
make_isr(3, 3, -1);
make_isr(4, 4, -1);
make_isr(5, 5, -1);
make_isr(6, 6, -1);
make_isr(7, 7, -1);
make_isr(8, 8, -1);
make_isr(9, 9, -1);
make_isr(10, 10, -1);
make_isr(11, 11, -1);
make_isr(12, 12, -1);
make_isr(13, 13, -1);
make_isr(14, 14, -1);
make_isr(15, 15, -1);
make_isr(16, 16, -1);
make_isr(17, 17, -1);
make_isr(18, 18, -1);
make_isr(19, 19, -1);
make_isr(20, 20, -1);
make_isr(21, 21, -1);
make_isr(22, 22, -1);
make_isr(23, 23, -1);
make_isr(24, 24, -1);
make_isr(25, 25, -1);
make_isr(26, 26, -1);
make_isr(27, 27, -1);
make_isr(28, 28, -1);
make_isr(29, 29, -1);
make_isr(30, 30, -1);
make_isr(31, 31, -1);

make_isr(32, -1, 0);
make_isr(33, -1, 1);
make_isr(34, -1, 2);
make_isr(35, -1, 3);
make_isr(36, -1, 4);
make_isr(37, -1, 5);
make_isr(38, -1, 6);
make_isr(39, -1, 7);
make_isr(40, -1, 8);
make_isr(41, -1, 9);
make_isr(42, -1, 10);
make_isr(43, -1, 11);
make_isr(44, -1, 12);
make_isr(45, -1, 13);
make_isr(46, -1, 14);
make_isr(47, -1, 15);

make_isr(48, -1, -1);
make_isr(49, -1, -1);
make_isr(50, -1, -1);
make_isr(51, -1, -1);
make_isr(52, -1, -1);
make_isr(53, -1, -1);
make_isr(54, -1, -1);
make_isr(55, -1, -1);
make_isr(56, -1, -1);
make_isr(57, -1, -1);
make_isr(58, -1, -1);
make_isr(59, -1, -1);
make_isr(60, -1, -1);
make_isr(61, -1, -1);
make_isr(62, -1, -1);
make_isr(63, -1, -1);
make_isr(64, -1, -1);
make_isr(65, -1, -1);
make_isr(66, -1, -1);
make_isr(67, -1, -1);
make_isr(68, -1, -1);
make_isr(69, -1, -1);
make_isr(70, -1, -1);
make_isr(71, -1, -1);
make_isr(72, -1, -1);
make_isr(73, -1, -1);
make_isr(74, -1, -1);
make_isr(75, -1, -1);
make_isr(76, -1, -1);
make_isr(77, -1, -1);
make_isr(78, -1, -1);
make_isr(79, -1, -1);
make_isr(80, -1, -1);
make_isr(81, -1, -1);
make_isr(82, -1, -1);
make_isr(83, -1, -1);
make_isr(84, -1, -1);
make_isr(85, -1, -1);
make_isr(86, -1, -1);
make_isr(87, -1, -1);
make_isr(88, -1, -1);
make_isr(89, -1, -1);
make_isr(90, -1, -1);
make_isr(91, -1, -1);
make_isr(92, -1, -1);
make_isr(93, -1, -1);
make_isr(94, -1, -1);
make_isr(95, -1, -1);
make_isr(96, -1, -1);
make_isr(97, -1, -1);
make_isr(98, -1, -1);
make_isr(99, -1, -1);
make_isr(100, -1, -1);
make_isr(101, -1, -1);
make_isr(102, -1, -1);
make_isr(103, -1, -1);
make_isr(104, -1, -1);
make_isr(105, -1, -1);
make_isr(106, -1, -1);
make_isr(107, -1, -1);
make_isr(108, -1, -1);
make_isr(109, -1, -1);
make_isr(110, -1, -1);
make_isr(111, -1, -1);
make_isr(112, -1, -1);
make_isr(113, -1, -1);
make_isr(114, -1, -1);
make_isr(115, -1, -1);
make_isr(116, -1, -1);
make_isr(117, -1, -1);
make_isr(118, -1, -1);
make_isr(119, -1, -1);
make_isr(120, -1, -1);
make_isr(121, -1, -1);
make_isr(122, -1, -1);
make_isr(123, -1, -1);
make_isr(124, -1, -1);
make_isr(125, -1, -1);
make_isr(126, -1, -1);
make_isr(127, -1, -1);
make_isr(128, -1, -1);
make_isr(129, -1, -1);
make_isr(130, -1, -1);
make_isr(131, -1, -1);
make_isr(132, -1, -1);
make_isr(133, -1, -1);
make_isr(134, -1, -1);
make_isr(135, -1, -1);
make_isr(136, -1, -1);
make_isr(137, -1, -1);
make_isr(138, -1, -1);
make_isr(139, -1, -1);
make_isr(140, -1, -1);
make_isr(141, -1, -1);
make_isr(142, -1, -1);
make_isr(143, -1, -1);
make_isr(144, -1, -1);
make_isr(145, -1, -1);
make_isr(146, -1, -1);
make_isr(147, -1, -1);
make_isr(148, -1, -1);
make_isr(149, -1, -1);
make_isr(150, -1, -1);
make_isr(151, -1, -1);
make_isr(152, -1, -1);
make_isr(153, -1, -1);
make_isr(154, -1, -1);
make_isr(155, -1, -1);
make_isr(156, -1, -1);
make_isr(157, -1, -1);
make_isr(158, -1, -1);
make_isr(159, -1, -1);
make_isr(160, -1, -1);
make_isr(161, -1, -1);
make_isr(162, -1, -1);
make_isr(163, -1, -1);
make_isr(164, -1, -1);
make_isr(165, -1, -1);
make_isr(166, -1, -1);
make_isr(167, -1, -1);
make_isr(168, -1, -1);
make_isr(169, -1, -1);
make_isr(170, -1, -1);
make_isr(171, -1, -1);
make_isr(172, -1, -1);
make_isr(173, -1, -1);
make_isr(174, -1, -1);
make_isr(175, -1, -1);
make_isr(176, -1, -1);
make_isr(177, -1, -1);
make_isr(178, -1, -1);
make_isr(179, -1, -1);
make_isr(180, -1, -1);
make_isr(181, -1, -1);
make_isr(182, -1, -1);
make_isr(183, -1, -1);
make_isr(184, -1, -1);
make_isr(185, -1, -1);
make_isr(186, -1, -1);
make_isr(187, -1, -1);
make_isr(188, -1, -1);
make_isr(189, -1, -1);
make_isr(190, -1, -1);
make_isr(191, -1, -1);
make_isr(192, -1, -1);
make_isr(193, -1, -1);
make_isr(194, -1, -1);
make_isr(195, -1, -1);
make_isr(196, -1, -1);
make_isr(197, -1, -1);
make_isr(198, -1, -1);
make_isr(199, -1, -1);
make_isr(200, -1, -1);
make_isr(201, -1, -1);
make_isr(202, -1, -1);
make_isr(203, -1, -1);
make_isr(204, -1, -1);
make_isr(205, -1, -1);
make_isr(206, -1, -1);
make_isr(207, -1, -1);
make_isr(208, -1, -1);
make_isr(209, -1, -1);
make_isr(210, -1, -1);
make_isr(211, -1, -1);
make_isr(212, -1, -1);
make_isr(213, -1, -1);
make_isr(214, -1, -1);
make_isr(215, -1, -1);
make_isr(216, -1, -1);
make_isr(217, -1, -1);
make_isr(218, -1, -1);
make_isr(219, -1, -1);
make_isr(220, -1, -1);
make_isr(221, -1, -1);
make_isr(222, -1, -1);
make_isr(223, -1, -1);
make_isr(224, -1, -1);
make_isr(225, -1, -1);
make_isr(226, -1, -1);
make_isr(227, -1, -1);
make_isr(228, -1, -1);
make_isr(229, -1, -1);
make_isr(230, -1, -1);
make_isr(231, -1, -1);
make_isr(232, -1, -1);
make_isr(233, -1, -1);
make_isr(234, -1, -1);
make_isr(235, -1, -1);
make_isr(236, -1, -1);
make_isr(237, -1, -1);
make_isr(238, -1, -1);
make_isr(239, -1, -1);
make_isr(240, -1, -1);
make_isr(241, -1, -1);
make_isr(242, -1, -1);
make_isr(243, -1, -1);
make_isr(244, -1, -1);
make_isr(245, -1, -1);
make_isr(246, -1, -1);
make_isr(247, -1, -1);
make_isr(248, -1, -1);
make_isr(249, -1, -1);
make_isr(250, -1, -1);
make_isr(251, -1, -1);
make_isr(252, -1, -1);
make_isr(253, -1, -1);
make_isr(254, -1, -1);
make_isr(255, -1, -1);


uintptr_t isrs[256] = {
    get_isr(0),
    get_isr(1),
    get_isr(2),
    get_isr(3),
    get_isr(4),
    get_isr(5),
    get_isr(6),
    get_isr(7),
    get_isr(8),
    get_isr(9),
    get_isr(10),
    get_isr(11),
    get_isr(12),
    get_isr(13),
    get_isr(14),
    get_isr(15),
    get_isr(16),
    get_isr(17),
    get_isr(18),
    get_isr(19),
    get_isr(20),
    get_isr(21),
    get_isr(22),
    get_isr(23),
    get_isr(24),
    get_isr(25),
    get_isr(26),
    get_isr(27),
    get_isr(28),
    get_isr(29),
    get_isr(30),
    get_isr(31),
    get_isr(32),
    get_isr(33),
    get_isr(34),
    get_isr(35),
    get_isr(36),
    get_isr(37),
    get_isr(38),
    get_isr(39),
    get_isr(40),
    get_isr(41),
    get_isr(42),
    get_isr(43),
    get_isr(44),
    get_isr(45),
    get_isr(46),
    get_isr(47),
    get_isr(48),
    get_isr(49),
    get_isr(50),
    get_isr(51),
    get_isr(52),
    get_isr(53),
    get_isr(54),
    get_isr(55),
    get_isr(56),
    get_isr(57),
    get_isr(58),
    get_isr(59),
    get_isr(60),
    get_isr(61),
    get_isr(62),
    get_isr(63),
    get_isr(64),
    get_isr(65),
    get_isr(66),
    get_isr(67),
    get_isr(68),
    get_isr(69),
    get_isr(70),
    get_isr(71),
    get_isr(72),
    get_isr(73),
    get_isr(74),
    get_isr(75),
    get_isr(76),
    get_isr(77),
    get_isr(78),
    get_isr(79),
    get_isr(80),
    get_isr(81),
    get_isr(82),
    get_isr(83),
    get_isr(84),
    get_isr(85),
    get_isr(86),
    get_isr(87),
    get_isr(88),
    get_isr(89),
    get_isr(90),
    get_isr(91),
    get_isr(92),
    get_isr(93),
    get_isr(94),
    get_isr(95),
    get_isr(96),
    get_isr(97),
    get_isr(98),
    get_isr(99),
    get_isr(100),
    get_isr(101),
    get_isr(102),
    get_isr(103),
    get_isr(104),
    get_isr(105),
    get_isr(106),
    get_isr(107),
    get_isr(108),
    get_isr(109),
    get_isr(110),
    get_isr(111),
    get_isr(112),
    get_isr(113),
    get_isr(114),
    get_isr(115),
    get_isr(116),
    get_isr(117),
    get_isr(118),
    get_isr(119),
    get_isr(120),
    get_isr(121),
    get_isr(122),
    get_isr(123),
    get_isr(124),
    get_isr(125),
    get_isr(126),
    get_isr(127),
    get_isr(128),
    get_isr(129),
    get_isr(130),
    get_isr(131),
    get_isr(132),
    get_isr(133),
    get_isr(134),
    get_isr(135),
    get_isr(136),
    get_isr(137),
    get_isr(138),
    get_isr(139),
    get_isr(140),
    get_isr(141),
    get_isr(142),
    get_isr(143),
    get_isr(144),
    get_isr(145),
    get_isr(146),
    get_isr(147),
    get_isr(148),
    get_isr(149),
    get_isr(150),
    get_isr(151),
    get_isr(152),
    get_isr(153),
    get_isr(154),
    get_isr(155),
    get_isr(156),
    get_isr(157),
    get_isr(158),
    get_isr(159),
    get_isr(160),
    get_isr(161),
    get_isr(162),
    get_isr(163),
    get_isr(164),
    get_isr(165),
    get_isr(166),
    get_isr(167),
    get_isr(168),
    get_isr(169),
    get_isr(170),
    get_isr(171),
    get_isr(172),
    get_isr(173),
    get_isr(174),
    get_isr(175),
    get_isr(176),
    get_isr(177),
    get_isr(178),
    get_isr(179),
    get_isr(180),
    get_isr(181),
    get_isr(182),
    get_isr(183),
    get_isr(184),
    get_isr(185),
    get_isr(186),
    get_isr(187),
    get_isr(188),
    get_isr(189),
    get_isr(190),
    get_isr(191),
    get_isr(192),
    get_isr(193),
    get_isr(194),
    get_isr(195),
    get_isr(196),
    get_isr(197),
    get_isr(198),
    get_isr(199),
    get_isr(200),
    get_isr(201),
    get_isr(202),
    get_isr(203),
    get_isr(204),
    get_isr(205),
    get_isr(206),
    get_isr(207),
    get_isr(208),
    get_isr(209),
    get_isr(210),
    get_isr(211),
    get_isr(212),
    get_isr(213),
    get_isr(214),
    get_isr(215),
    get_isr(216),
    get_isr(217),
    get_isr(218),
    get_isr(219),
    get_isr(220),
    get_isr(221),
    get_isr(222),
    get_isr(223),
    get_isr(224),
    get_isr(225),
    get_isr(226),
    get_isr(227),
    get_isr(228),
    get_isr(229),
    get_isr(230),
    get_isr(231),
    get_isr(232),
    get_isr(233),
    get_isr(234),
    get_isr(235),
    get_isr(236),
    get_isr(237),
    get_isr(238),
    get_isr(239),
    get_isr(240),
    get_isr(241),
    get_isr(242),
    get_isr(243),
    get_isr(244),
    get_isr(245),
    get_isr(246),
    get_isr(247),
    get_isr(248),
    get_isr(249),
    get_isr(250),
    get_isr(251),
    get_isr(252),
    get_isr(253),
    get_isr(254),
    get_isr(255),
};

#undef make_isr
#undef get_isr
