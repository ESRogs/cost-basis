open Jest;
open Lots;

let product = "XYZ";
let sellPrice = 1000.;
let dateSold = ISOString("2017-07-01");
let tl = {
  product,
  size: 5.,
  buyPrice: 1000.,
  dateAcquired: ISOString("2017-01-01"),
};
let lots = [tl];

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
    let expectedSales = [{
      taxLot: { ...tl, size: 1.5 },
      sellPrice,
      dateSold,
    }];
    let expectedLeftovers = [{ ...tl, size: 3.5 }];
    let expectedResult = Success(expectedSales, expectedLeftovers);
    expect(sell(size, sellPrice, dateSold, lots)) |> toEqual(expectedResult)
  });
});
