import requests
import numbers

# def test_get_populations(nest_simulation):
#     r = requests.get("http://localhost:8080/nest/populations")
#     populations = r.json()

#     assert(isinstance(populations, list))
#     for population in populations:
#         assert(isinstance(population, numbers.Number))
#         assert(populations.count(population) == 1)

# def test_get_population_gids(nest_simulation):
#     populations_request = requests.get("http://localhost:8080/nest/populations")

#     assert(populations_request.status_code == 200)
#     populations = populations_request.json()

#     combined = []
#     for population in populations:
#         population_gids_request = requests.get("http://localhost:8080/nest/population/${}/gids".format(population))
#         assert(population_gids_request.status_code == 200)
#         population_gids = population_gids_request.json()
#         assert(isinstance(population_gids, list))
#         for gid in population_gids:
#             assert(isinstance(gid, numbers.Integral))
#         combined.extend(population_gids)

#     gids_request = requests.get("http://localhost:8080/nest/gids")
#     assert(gids_request.status_code == 200)
#     gids = gids_request.json()
#     assert(combined.sort() == gids.sort())
