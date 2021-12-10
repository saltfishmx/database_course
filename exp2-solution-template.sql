-- 姓名：XXX
-- 学号：XXX
-- 提交前请确保本次实验独立完成，若有参考请注明并致谢。
select * from customer;
select * from employee;
select * from orderdetail;
select * from ordermaster;
select * from product
SET SQL_SAFE_UPDATES = 0
update employee set salary = salary + 200 where department = '业务科'
-- ____________________________________________________________________________________________________________________________________________________________________________________________________________
-- BEGIN Q1.1
DELIMITER //
create procedure product_related_info(in productname varchar(40))
	begin
		select c.customerNo as cNO,c.customerName as cName,od.orderNo as oNo,od.quantity as quantity,(quantity*price) as totalprice
        from product p,customer c,ordermaster om,orderdetail od
        where productname = p.productName and p.productNo = od.productNo and od.orderNo = om.orderNo and om.customerNo = c.customerNo
        order by totalprice desc;
    end //
DELIMITER ;
call product_related_info('32M DRAM');
-- END Q1.1

-- ____________________________________________________________________________________________________________________________________________________________________________________________________________
-- BEGIN Q1.2
DELIMITER //
create procedure earlier_employee(in employee_no char(8))
	begin
		select e1.employeeNo as employeeNo,e1.employeeName as employeeName,e1.gender as gender, e1.hireDate as hireDate,e1.department as department
        from employee e1,
        (
			select department,hireDate
            from employee e
            where employeeNo = employee_no
        )e2
        where    e1.hireDate < e2.hireDate and e1.department = e2.department;
    end//
    
DELIMITER ;
call earlier_employee('E2008005');
-- END Q1.2
set global log_bin_trust_function_creators=1;
drop function averageprice;
-- ____________________________________________________________________________________________________________________________________________________________________________________________________________
-- BEGIN Q2.1
DELIMITER //
create function averageprice(product_name varchar(40))
	returns numeric(7,2)
    begin
    declare average_price numeric(7,2);
		select avg(price) into average_price
        from orderdetail od,product p
        where od.productNo = p.productNo and p.productName = product_name;
	return average_price;
    end//
DELIMITER ; 
select productName,averageprice(productName)
from product
group by productName; 
   
-- END Q2.1
set global log_bin_trust_function_creators=1;
-- ____________________________________________________________________________________________________________________________________________________________________________________________________________
-- BEGIN Q2.2
DELIMITER //
create function totalsalequantity(productno char(9))
	returns integer
    begin 
    declare tquantity integer;
		select sum(quantity) into tquantity
        from orderdetail od
        where productno = od.productNo;
	return tquantity;
    end//
DELIMITER ;
select od.productNo as productNo,productName as pname,totalsalequantity(od.productNo) as total_sale_quantity
from orderdetail od,product p
where od.productNo = p.productNo and totalsalequantity(od.productNo)>4
group by od.productNo;
-- END Q2.2

-- ____________________________________________________________________________________________________________________________________________________________________________________________________________
-- BEGIN Q3.1
DELIMITER //
drop trigger product_price//
create trigger product_price before insert on product 
for each row
begin
if (new.productPrice>1000)
	then set new.productPrice = 1000;
    end if;
end//
DELIMITER ;
insert into product values('P20080004','testname','testclass',1300);
delete from product where  productNo = 'P20080004';
-- END Q3.1

-- ____________________________________________________________________________________________________________________________________________________________________________________________________________
-- BEGIN Q3.2
DELIMITER //
drop trigger addsalary//
create trigger addsalary before insert on ordermaster
for each row
begin
	if (new.employeeNo in (select employeeNo from employee where hiredate < '1992-01-01 00:00:00'))
    then update employee
		set employee.salary = 1.08 * employee.salary
        where new.employeeNo = employee.employeeNo;
	else 
		update employee
		set employee.salary = 1.05 * employee.salary
        where new.employeeNo = employee.employeeNo;
	end if;
end//

DELIMITER ;
insert into employee values('E2008006','test','F','1970-11-06 00:00:00','test',null,'1990-11-18 00:00:00','test','test','10000');
insert into ordermaster values('200806120002', 'C20050001', 'E2008006', '2008-06-12 00:00:00', '0.00', 'I000000010')
-- END Q3.2

