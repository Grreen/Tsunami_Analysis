import netCDF4

saveNewStyle = False

filename = 'YOUR_PATH'
outputfile = 'YOUR_PATH'

ncfile = netCDF4.Dataset(filename, 'r')
depth = ncfile.variables['elevation'][:,:]

lat = ncfile.variables['lat'][:].data
lon = ncfile.variables['lon'][:].data

ncfile.close()

print('{} {}'.format(max(lat), min(lat)))
print('{} {}'.format(max(lon), min(lon)))

stepX = 1
stepY = 1

with open(outputfile, 'w') as res:
    if saveNewStyle:
        arXX = range(0, len(lon), stepX)
        arYY = range(0, len(lat), stepY)
        print('{:.0f} {:.0f}'.format(len(arXX), len(arYY)))
        
        res.write('{0:.0f} {1:.0f}\n'.format(len(arXX), len(arYY)))
        res.write('{0:.4f} {1:.4f}\n'.format(lon[arXX[-1]], lon[arXX[0]]))
        res.write('{0:.4f} {1:.4f}\n'.format(lat[arYY[-1]], lat[arYY[0]]))

        for yy in arYY[::-1]:
            for xx in arXX:
                res.write('{:4f} '.format(depth[yy, xx]))
    else:
        for xx in range(0, len(lat), stepX):
            x = lat[xx]
            for yy in range(0, len(lon), stepY):
                y = lon[yy]
                res.write('{1:.4f} {0:.4f} {2} \n'.format(x, y, depth[xx, yy]))
