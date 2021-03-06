open Jest;
open Lots;

let product = "XYZ";

let tl1 = {
  product,
  size: 1.,
  buyPrice: 1000.,
  dateAcquired: ISOString("2017-01-01"),
};

let tl2 = {
  product,
  size: 4.,
  buyPrice: 1000.,
  dateAcquired: ISOString("2017-01-02"),
};

let tl3 = {
  product,
  size: 2.5,
  buyPrice: 500.,
  dateAcquired: ISOString("2017-01-03"),
};

let lots = [tl1, tl2, tl3];

exception Unpossible(string);

let sellAndExpectSuccess = (size, sellPrice, dateSold, lots) => {
  let result = sell(size, sellPrice, dateSold, lots);
  switch (result) {
  | Success(sales, lotsRemaining) => (sales, lotsRemaining)
  | _ => raise(Unpossible("sale should succeed!"))
  }
};

describe("match lots", () => {
  open Expect;

  test("negative size", () => {
    let result = sell(-1., 1000., ISOString("2017-07-01"), lots);
    expect(result)
    |> toBe(InvalidSize)
  });

  test("zero size", () => {
    let result = sell(0., 1000., ISOString("2017-07-01"), lots);
    expect(result)
    |> toBe(InvalidSize)
  });

  test("0.4 sold", () => {
    let result = sell(0.4, 1000., ISOString("2017-07-01"), lots);
    let expectedSales = [{
      taxLot: { ...tl1, size: 0.4 },
      sellPrice: 1000.,
      dateSold: ISOString("2017-07-01"),
    }];
    let expectedLotsRemaining = [{ ...tl1, size: 0.6 }, tl2, tl3];
    expect(result)
    |> toEqual(Success(expectedSales, expectedLotsRemaining))
  });

  test("1.0 sold", () => {
    let result = sell(1.0, 1000., ISOString("2017-07-01"), lots);
    let expectedSales = [{
      taxLot: tl1,
      sellPrice: 1000.,
      dateSold: ISOString("2017-07-01"),
    }];
    let expectedLotsRemaining = [tl2, tl3];
    expect(result)
    |> toEqual(Success(expectedSales, expectedLotsRemaining))
  });

  test("1.7 sold", () => {
    let result = sell(1.7, 1000., ISOString("2017-07-01"), lots);
    let sale1 = {
      taxLot: tl1,
      sellPrice: 1000.,
      dateSold: ISOString("2017-07-01"),
    };
    let sale2 = {
      ...sale1,
      taxLot: { ...tl2, size: 0.7 },
    };
    let expectedSales = [sale1, sale2];
    let expectedLotsRemaining = [{ ...tl2, size: 3.3 }, tl3];
    expect(result)
    |> toEqual(Success(expectedSales, expectedLotsRemaining))
  });

  test("7.0 sold", () => {
    let result = sell(7., 1000., ISOString("2017-07-01"), lots);
    let sellPrice = 1000.;
    let dateSold = ISOString("2017-07-01");
    let expectedSales = [
      { taxLot: tl1, sellPrice, dateSold },
      { taxLot: tl2, sellPrice, dateSold },
      { taxLot: { ...tl3, size: 2. }, sellPrice, dateSold },
    ];
    let expectedLotsRemaining = [{ ...tl3, size: 0.5 }];
    expect(result)
    |> toEqual(Success(expectedSales, expectedLotsRemaining))
  });

  test("all sold", () => {
    let result = sell(7.5, 1000., ISOString("2017-07-01"), lots);
    let sellPrice = 1000.;
    let dateSold = ISOString("2017-07-01");
    let expectedSales = [
      { taxLot: tl1, sellPrice, dateSold },
      { taxLot: tl2, sellPrice, dateSold },
      { taxLot: tl3, sellPrice, dateSold },
    ];
    let expectedLotsRemaining = [];
    expect(result)
    |> toEqual(Success(expectedSales, expectedLotsRemaining))
  });

  test("insufficient lots", () => {
    let result = sell(7.6, 1000., ISOString("2017-07-01"), lots);
    expect(result)
    |> toEqual(InsufficientTaxLots)
  });

  test("6.0 sold across three sales", () => {
    let (sales1, lotsRemaining1) = sellAndExpectSuccess(2., 1000., ISOString("2017-07-01"), lots);
    let (sales2, lotsRemaining2) = sellAndExpectSuccess(2., 1000., ISOString("2017-07-01"), lotsRemaining1);
    let (sales3, lotsRemaining3) = sellAndExpectSuccess(2., 1000., ISOString("2017-07-01"), lotsRemaining2);
    let sellPrice = 1000.;
    let dateSold = ISOString("2017-07-01");

    let expectedSales1 = [
      { taxLot: tl1, sellPrice, dateSold },
      { taxLot: { ...tl2, size: 1. }, sellPrice, dateSold },
    ];
    let expectedLotsRemaining1 = [
      { ...tl2, size: 3. },
      tl3,
    ];

    let expectedSales2 = [ { taxLot: { ...tl2, size: 2. }, sellPrice, dateSold } ];
    let expectedLotsRemaining2 = [
      { ...tl2, size: 1. },
      tl3,
    ];

    let expectedSales3 = [
      { taxLot: { ...tl2, size: 1. }, sellPrice, dateSold },
      { taxLot: { ...tl3, size: 1. }, sellPrice, dateSold },
    ];
    let expectedLotsRemaining3 = [ { ...tl3, size: 1.5 } ];

    expect([
      (sales1, lotsRemaining1),
      (sales2, lotsRemaining2),
      (sales3, lotsRemaining3),
    ]) |> toEqual([
      (expectedSales1, expectedLotsRemaining1),
      (expectedSales2, expectedLotsRemaining2),
      (expectedSales3, expectedLotsRemaining3),
    ])
  });
});
