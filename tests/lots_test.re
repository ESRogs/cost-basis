open Jest;
open Lots;

let product = "XYZ";
let sellPrice = 1000.;
let dateSold = ISOString("2017-07-01");
let tl1 = {
  product,
  size: 1.,
  buyPrice: 1000.,
  dateAcquired: ISOString("2017-01-01"),
};
let tl2 = {
  ...tl1,
  size: 4.,
  dateAcquired: ISOString("2017-01-02"),
};
let lots = [tl1, tl2];

describe("match lots", () => {
  open Expect;

  test("negative size", () => {
    let size = -1.;
    expect(sell(size, sellPrice, dateSold, lots)) |> toBe(InvalidSize)
  });

  test("zero size", () => {
    let size = 0.;
    expect(sell(size, sellPrice, dateSold, lots)) |> toEqual(Success([], lots))
  });

  test("1.5 sold", () => {
    let size = 1.5;
    let sale1 = {
      taxLot: tl1,
      sellPrice,
      dateSold,
    };
    let sale2 = {
      ...sale1,
      taxLot: { ...tl2, size: 0.5 },
    };
    let expectedSales = [sale1, sale2];
    let expectedLeftovers = [{ ...tl2, size: 3.5 }];
    let expectedResult = Success(expectedSales, expectedLeftovers);
    expect(sell(size, sellPrice, dateSold, lots)) |> toEqual(expectedResult)
  });

  test("insufficient lots", () => {
    let size = 9.;
    expect(sell(size, sellPrice, dateSold, lots)) |> toEqual(InsufficientTaxLots)
  });
});
