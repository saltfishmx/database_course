-- 姓名：孔孟荀
-- 学号：181180050
-- 提交前请确保本次实验独立完成，若有参考请注明并致谢。
USE `phonemon`;
-- ____________________________________________________________________________________________________________________________________________________________________________________________________________
-- BEGIN Q1
SELECT COUNT(*) as speciesCount FROM (SELECT * FROM species WHERE description like '%this%') qs1;
-- END Q1

-- ____________________________________________________________________________________________________________________________________________________________________________________________________________
-- BEGIN Q2
select username,sum(power) as totalPhonemonPower  
from (select id,username from player 
	 where username in  ('Cook','Hughes') ) t , phonemon p 
where p.player = t.id    
group by  username;
-- END Q2

-- ____________________________________________________________________________________________________________________________________________________________________________________________________________
-- BEGIN Q3
select title,numberOfPlayers
from(
select team,count(*) as numberOfPlayers
from player
where team is not null
group by team
) a, team t
where a.team = t.id
order by numberOfPlayers DESC;
-- END Q3

-- ____________________________________________________________________________________________________________________________________________________________________________________________________________
-- BEGIN Q4
select s.id as idSpecies,title
from(
select id 
from type 
where title = 'grass'
) typeid, species s
where s.type1 = typeid.id or s.type2 = typeid.id;
-- END Q4

-- ____________________________________________________________________________________________________________________________________________________________________________________________________________
-- BEGIN Q5
select player.id as idPlayer,username
from player 
where player.id not in (
select distinct player 
from(
select id
from item
where type = 'f'
) itemid , purchase p
where itemid.id = p.item
);
-- END Q5

-- ____________________________________________________________________________________________________________________________________________________________________________________________________________
-- BEGIN Q6
select level,sum(quantity*price) as totalAmountSpentByAllPlayersAtLevel
from(
select level, id
from player 
)t, item i, purchase p 
where p.player=t.id and p.item = i.id
group by level
order by totalAmountSpentByAllPlayersAtLevel desc;
-- END Q6

-- ____________________________________________________________________________________________________________________________________________________________________________________________________________
-- BEGIN Q7
select item,title,max(TimesPurchased) as numTimesPurchased
from(
select item , count(*) as TimesPurchased
from purchase 
group by item
)t,item i
where t.item = i.id and t.TimesPurchased in 
(select max(t1.TimesPurchased) 
from(
select item , count(*) as TimesPurchased
from purchase 
group by item
)t1);
-- 购买次数不是数量             
-- END Q7

-- ____________________________________________________________________________________________________________________________________________________________________________________________________________
-- BEGIN Q8
select playerID,username, sum(quantity) as numberDistinctFoodItemsPurchased
from(
select id as playerID,username
from player p
where not exists(
select id 
from item
where item.type = 'F' and not exists(
select *
from purchase
where purchase.item = item.id and purchase.player = p.id
-- p购买过的item的集合（where下三行）
)
-- item 里是食物但是没有被p购买过的集合（where内）
)
-- 找不到上面这个集合的p   是所要的p
)t,purchase
where playerID = player 
group by player;
-- 这个题一直在想要用group by ，直到把问题一分为二，先解决这样的玩家的编号（而不是同时把买的食物数量也找到），才理清。
-- END Q8

-- ____________________________________________________________________________________________________________________________________________________________________________________________________________
-- BEGIN Q9
select t1.distance as distanceX,count(*) as numberOfPhonemonPairs
from(
select round((round(sqrt( power((p1.latitude-p2.latitude),2) + power((p1.longitude-p2.longitude),2)),4)*100),2) as distance
from phonemon p1,phonemon p2
where p1.id>p2.id
)t1
where t1.distance in(
select max(t2.distance) from(
select round((round(sqrt( power((p1.latitude-p2.latitude),2) + power((p1.longitude-p2.longitude),2)),4)*100),2) as distance
from phonemon p1,phonemon p2
where p1.id>p2.id
)t2
);
-- END Q9

-- ____________________________________________________________________________________________________________________________________________________________________________________________________________
-- BEGIN Q10
select username,title as typeTitle
from player p,type t
where not exists(
select * 
from species s
where (s.type1 = t.id or s.type2 = t.id) and not exists(
select *
from phonemon ph
where ph.player = p.id and ph.species = s.id
)
);
-- END Q10